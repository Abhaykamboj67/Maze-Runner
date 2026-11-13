
#include "HttpServer.h"

#include <winsock2.h>
#include <ws2tcpip.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

using namespace std;

// Read a file from disk and return its contents
string readFile(const string& filePath) {

    ifstream file(filePath, ios::binary);

    if (!file.is_open()) {
        return "";
    }

    stringstream buffer;
    buffer << file.rdbuf();

    return buffer.str();
}

// Determine content type from file extension
string getContentType(const string& path) {

    if (path.find(".html") != string::npos) {
        return "text/html";
    }
    if (path.find(".css") != string::npos) {
        return "text/css";
    }
    if (path.find(".js") != string::npos) {
        return "application/javascript";
    }

    return "text/plain";
}

// Send complete HTTP response
void sendResponse(SOCKET client, int status,
                  const string& contentType,
                  const string& content) {

    string statusText = (status == 200) ? "OK" :
                        (status == 404) ? "Not Found" :
                        "Bad Request";

    string response =
        "HTTP/1.1 " + to_string(status) + " " + statusText + "\r\n"
        "Content-Type: " + contentType + "\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "Access-Control-Allow-Methods: GET, OPTIONS\r\n"
        "Access-Control-Allow-Headers: Content-Type\r\n"
        "Content-Length: " + to_string(content.size()) + "\r\n"
        "Connection: close\r\n\r\n" +
        content;

    int sent = 0;

    while (sent < static_cast<int>(response.size())) {

        int result = send(
            client,
            response.c_str() + sent,
            static_cast<int>(response.size()) - sent,
            0
        );

        if (result == SOCKET_ERROR || result == 0) {
            break;
        }

        sent += result;
    }
}

// Overload for JSON responses (backward compatible)
void sendJSON(SOCKET client, int status,
              const string& content) {

    sendResponse(client, status, "application/json", content);
}

// Convert maze grid into JSON
string mazeToJSON(Maze& maze) {

    const auto& grid = maze.getGrid();

    pair<int, int> start = maze.getStart();
    pair<int, int> finish = maze.getFinish();

    stringstream json;

    json << "{\"grid\":[";

    for (size_t i = 0; i < grid.size(); i++) {

        json << "[";

        for (size_t j = 0; j < grid[i].size(); j++) {

            json << grid[i][j];

            if (j < grid[i].size() - 1) {
                json << ",";
            }
        }

        json << "]";

        if (i < grid.size() - 1) {
            json << ",";
        }
    }

    json << "],";

    json << "\"start\":[" << start.first
         << "," << start.second << "],";

    json << "\"finish\":[" << finish.first
         << "," << finish.second << "]}";

    return json.str();
}

// Convert path into JSON
string pathToJSON(
    const vector<pair<int, int>>& path,
    const string& algorithm
) {

    stringstream json;

    json << "{\"algorithm\":\"" << algorithm << "\",";
    json << "\"path\":[";

    for (size_t i = 0; i < path.size(); i++) {

        json << "[" << path[i].first
             << "," << path[i].second << "]";

        if (i < path.size() - 1) {
            json << ",";
        }
    }

    json << "],";

    json << "\"length\":"
         << (path.empty() ? 0 : path.size() - 1);

    json << "}";

    return json.str();
}

// Start HTTP server
void startServer(Maze& maze, int port) {

    WSADATA wsaData;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {

        cout << "Winsock initialization failed!\n";
        return;
    }

    SOCKET serverSocket = socket(
        AF_INET,
        SOCK_STREAM,
        IPPROTO_TCP
    );

    if (serverSocket == INVALID_SOCKET) {

        cout << "Socket creation failed!\n";
        WSACleanup();
        return;
    }

    sockaddr_in serverAddress{};

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if (bind(
        serverSocket,
        (sockaddr*)&serverAddress,
        sizeof(serverAddress)
    ) == SOCKET_ERROR) {

        cout << "Port binding failed! Is port " << port << " already in use?\n";

        closesocket(serverSocket);
        WSACleanup();

        return;
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {

        cout << "Server listening failed!\n";

        closesocket(serverSocket);
        WSACleanup();

        return;
    }

    cout << "\n================================\n";
    cout << "       MAZE RUNNER SERVER\n";
    cout << "================================\n";
    cout << "Server running on port " << port << "\n";
    cout << "Open: http://localhost:" << port << "\n";
    cout << "API:  http://localhost:" << port << "/api/maze\n";
    cout << "Opening browser...\n";
    cout << "Press Ctrl+C to stop.\n\n";

    // Auto-open browser on Windows
    system(("start http://localhost:" + to_string(port)).c_str());

    while (true) {

        SOCKET clientSocket = accept(
            serverSocket,
            nullptr,
            nullptr
        );

        if (clientSocket == INVALID_SOCKET) {
            continue;
        }

        char buffer[4096] = {};

        int received = recv(
            clientSocket,
            buffer,
            sizeof(buffer) - 1,
            0
        );

        if (received > 0) {

            string request(buffer, received);

            string method;
            string path;

            stringstream requestStream(request);

            requestStream >> method >> path;

            cout << method << " " << path << endl;

            // CORS preflight
            if (method == "OPTIONS") {

                sendJSON(clientSocket, 200, "{}");
            }

            // === API Routes ===

            else if (method == "GET" &&
                     path == "/api/maze") {

                sendJSON(
                    clientSocket,
                    200,
                    mazeToJSON(maze)
                );
            }

            else if (method == "GET" &&
                     path == "/api/new") {

                maze.generateMaze();

                sendJSON(
                    clientSocket,
                    200,
                    mazeToJSON(maze)
                );
            }

            else if (method == "GET" &&
                     path.find("/api/solve") == 0) {

                vector<pair<int, int>> solution;

                string algorithm = "bfs";

                if (path.find("algorithm=dfs") != string::npos) {

                    algorithm = "dfs";
                    solution = maze.solveDFS();
                }

                else {

                    algorithm = "bfs";
                    solution = maze.solveBFS();
                }

                sendJSON(
                    clientSocket,
                    200,
                    pathToJSON(solution, algorithm)
                );
            }

            // === Static File Routes ===

            else if (method == "GET" &&
                     (path == "/" || path == "/index.html")) {

                string content = readFile("web/index.html");

                if (content.empty()) {
                    sendResponse(clientSocket, 404, "text/plain",
                                 "index.html not found");
                } else {
                    sendResponse(clientSocket, 200, "text/html", content);
                }
            }

            else if (method == "GET" &&
                     path == "/style.css") {

                string content = readFile("web/style.css");

                if (content.empty()) {
                    sendResponse(clientSocket, 404, "text/plain",
                                 "style.css not found");
                } else {
                    sendResponse(clientSocket, 200, "text/css", content);
                }
            }

            else if (method == "GET" &&
                     path == "/script.js") {

                string content = readFile("web/script.js");

                if (content.empty()) {
                    sendResponse(clientSocket, 404, "text/plain",
                                 "script.js not found");
                } else {
                    sendResponse(clientSocket, 200, "application/javascript",
                                 content);
                }
            }

            // === 404 ===

            else {

                sendJSON(
                    clientSocket,
                    404,
                    "{\"error\":\"Endpoint not found\"}"
                );
            }
        }

        closesocket(clientSocket);
    }

    closesocket(serverSocket);
    WSACleanup();
}
