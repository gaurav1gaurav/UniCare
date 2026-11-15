#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_NODES 50
#define INF 999999.0
#define NAME_LEN 90

// Structure to represent an edge in the adjacency list
typedef struct Edge {
    int dest;
    double weight; // Distance in kilometers
    struct Edge* next;
} Edge;

// Structure for graph nodes with GPS coordinates
typedef struct {
    char name[NAME_LEN];
    bool is_hospital;
    double lat;
    double lon;
    Edge* head;
} GraphNode;

// Structure for the graph
typedef struct {
    GraphNode nodes[MAX_NODES];
    int node_count;
} Graph;

// Initialize the graph
void initGraph(Graph* g) {
    g->node_count = 0;
    for (int i = 0; i < MAX_NODES; i++) {
        g->nodes[i].head = NULL;
        g->nodes[i].is_hospital = false;
        g->nodes[i].name[0] = '\0';
        g->nodes[i].lat = 0.0;
        g->nodes[i].lon = 0.0;
    }
}

// Find node index by name, or add it with GPS coordinates if it doesn't exist
int findOrAddNode(Graph* g, const char* name, bool is_hospital, double lat, double lon) {
    for (int i = 0; i < g->node_count; i++) {
        if (strcmp(g->nodes[i].name, name) == 0) {
            if (is_hospital) {
                g->nodes[i].is_hospital = true;
            }
            return i;
        }
    }
    
    if (g->node_count < MAX_NODES) {
        int idx = g->node_count++;
        strcpy(g->nodes[idx].name, name);
        g->nodes[idx].is_hospital = is_hospital;
        g->nodes[idx].lat = lat;
        g->nodes[idx].lon = lon;
        g->nodes[idx].head = NULL;
        return idx;
    }
    return -1;
}

// Add a bidirectional road between two locations
void addRoad(Graph* g, const char* u_name, const char* v_name, double weight, 
             bool u_is_hosp, double u_lat, double u_lon, 
             bool v_is_hosp, double v_lat, double v_lon) {
    int u = findOrAddNode(g, u_name, u_is_hosp, u_lat, u_lon);
    int v = findOrAddNode(g, v_name, v_is_hosp, v_lat, v_lon);

    if (u == -1 || v == -1) return;

    // Add edge u -> v
    Edge* edge1 = (Edge*)malloc(sizeof(Edge));
    edge1->dest = v;
    edge1->weight = weight;
    edge1->next = g->nodes[u].head;
    g->nodes[u].head = edge1;

    // Add edge v -> u
    Edge* edge2 = (Edge*)malloc(sizeof(Edge));
    edge2->dest = u;
    edge2->weight = weight;
    edge2->next = g->nodes[v].head;
    g->nodes[v].head = edge2;
}

// Helper to get weight between two connected nodes
double getEdgeWeight(Graph* g, int u, int v) {
    Edge* temp = g->nodes[u].head;
    while (temp != NULL) {
        if (temp->dest == v) {
            return temp->weight;
        }
        temp = temp->next;
    }
    return 0.0;
}

// Dijkstra's Algorithm implementation
void dijkstra(Graph* g, int start_idx, double dist[], int parent[]) {
    bool visited[MAX_NODES];

    for (int i = 0; i < g->node_count; i++) {
        dist[i] = INF;
        parent[i] = -1;
        visited[i] = false;
    }

    dist[start_idx] = 0.0;

    for (int count = 0; count < g->node_count - 1; count++) {
        double min = INF;
        int u = -1;

        for (int i = 0; i < g->node_count; i++) {
            if (!visited[i] && dist[i] < min) {
                min = dist[i];
                u = i;
            }
        }

        if (u == -1) break;

        visited[u] = true;

        Edge* temp = g->nodes[u].head;
        while (temp != NULL) {
            int v = temp->dest;
            double weight = temp->weight;

            if (!visited[v] && dist[u] != INF && dist[u] + weight < dist[v]) {
                dist[v] = dist[u] + weight;
                parent[v] = u;
            }
            temp = temp->next;
        }
    }
}

// Generate Google Maps-style HTML map file
void generateHTMLMap(Graph* g, int path[], int path_len, double total_dist, int start_idx, int dest_idx) {
    FILE *fp = fopen("unicare_map.html", "w");
    if (fp == NULL) {
        printf("Error creating HTML map file!\n");
        return;
    }

    fprintf(fp, "<!DOCTYPE html>\n<html>\n<head>\n");
    fprintf(fp, "<title>Uni-Care Emergency Route Map</title>\n");
    fprintf(fp, "<link rel=\"stylesheet\" href=\"https://unpkg.com/leaflet/dist/leaflet.css\" />\n");
    fprintf(fp, "<style>\n");
    fprintf(fp, "  body { font-family: Arial, sans-serif; margin: 0; padding: 0; background: #f4f6f9; }\n");
    fprintf(fp, "  #map { height: 85vh; width: 100%%; }\n");
    fprintf(fp, "  .header { background: #d9534f; color: white; padding: 15px; text-align: center; }\n");
    fprintf(fp, "</style>\n</head>\n<body>\n");

    fprintf(fp, "<div class=\"header\">\n");
    fprintf(fp, "  <h2>🚨 Uni-Care Emergency Ambulance Route 🚨</h2>\n");
    fprintf(fp, "  <p><b>From:</b> %s | <b>To:</b> %s | <b>Total Distance:</b> %.2f km</p>\n", 
            g->nodes[start_idx].name, g->nodes[dest_idx].name, total_dist);
    fprintf(fp, "</div>\n");
    fprintf(fp, "<div id=\"map\"></div>\n");

    fprintf(fp, "<script src=\"https://unpkg.com/leaflet/dist/leaflet.js\"></script>\n");
    fprintf(fp, "<script>\n");
    fprintf(fp, "  var map = L.map('map').setView([%.6f, %.6f], 14);\n", 
            g->nodes[start_idx].lat, g->nodes[start_idx].lon);
    
    fprintf(fp, "  L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', {\n");
    fprintf(fp, "    maxZoom: 19,\n");
    fprintf(fp, "    attribution: '© OpenStreetMap contributors'\n");
    fprintf(fp, "  }).addTo(map);\n\n");

    // Plot markers for all nodes
    for (int i = 0; i < g->node_count; i++) {
        fprintf(fp, "  L.marker([%.6f, %.6f]).addTo(map).bindPopup('<b>%s</b><br>%s');\n",
                g->nodes[i].lat, g->nodes[i].lon, g->nodes[i].name,
                g->nodes[i].is_hospital ? "🏥 Hospital" : (i == start_idx ? "🚀 Emergency Origin" : "📍 Junction"));
    }

    // Draw the Shortest Route Polyline
    fprintf(fp, "  var routeCoordinates = [\n");
    for (int i = path_len - 1; i >= 0; i--) {
        int idx = path[i];
        fprintf(fp, "    [%.6f, %.6f]%s\n", g->nodes[idx].lat, g->nodes[idx].lon, (i > 0) ? "," : "");
    }
    fprintf(fp, "  ];\n");

    fprintf(fp, "  var polyline = L.polyline(routeCoordinates, {color: 'red', weight: 6, opacity: 0.85}).addTo(map);\n");
    fprintf(fp, "  map.fitBounds(polyline.getBounds());\n");

    fprintf(fp, "</script>\n</body>\n</html>\n");
    fclose(fp);

    printf("✨ Interactive map saved as 'unicare_map.html' (Double-click to open in browser)!\n");
}

// Display route for a selected hospital index
void displayRouteToHospital(Graph* g, int start_idx, int dest_idx, double dist[], int parent[]) {
    if (dist[dest_idx] >= INF) {
        printf("\n❌ No reachable route found to %s.\n", g->nodes[dest_idx].name);
        return;
    }

    printf("\n=================================================================\n");
    printf("🚨 UNI-CARE AMBULANCE DISPATCH ROUTE 🚨\n");
    printf("=================================================================\n");
    printf("📍 Origin       : %s\n", g->nodes[start_idx].name);
    printf("🏥 Destination  : %s\n", g->nodes[dest_idx].name);
    printf("⏱️ Distance     : %.2f km\n", dist[dest_idx]);
    printf("-----------------------------------------------------------------\n");

    // Reconstruct path
    int path[MAX_NODES];
    int path_length = 0;
    int current = dest_idx;
    while (current != -1) {
        path[path_length++] = current;
        current = parent[current];
    }

    // Print step-by-step visual diagram
    printf("🗺️  STEP-BY-STEP ROUTE:\n\n");
    for (int i = path_length - 1; i >= 0; i--) {
        int node_idx = path[i];
        if (i == 0) {
            printf("  🏥 [ DESTINATION: %s ]\n", g->nodes[node_idx].name);
        } else if (i == path_length - 1) {
            printf("  🚀 [ START: %s ]\n", g->nodes[node_idx].name);
        } else {
            printf("  📍 [ Via: %s ]\n", g->nodes[node_idx].name);
        }

        if (i > 0) {
            double seg = getEdgeWeight(g, path[i], path[i - 1]);
            printf("         │\n");
            printf("         └──> (%.2f km) ──>\n", seg);
        }
    }
    printf("-----------------------------------------------------------------\n");

    // Generate Google Maps-style HTML file
    generateHTMLMap(g, path, path_length, dist[dest_idx], start_idx, dest_idx);
    printf("=================================================================\n");
}

int main() {
    Graph locality;
    initGraph(&locality);

    // --- Real Locality Graph around Graphic Era University (Clement Town, Dehradun) ---
    // GPS coordinates and realistic road distances matching your data
    
    // 1. Graphic Era University to Clement Town Chowk (Junction)
    addRoad(&locality, "Graphic Era University (Clement Town)", "Clement Town Chowk", 0.60,
            false, 30.2672, 77.9975,
            false, 30.2655, 78.0010);

    // 2. Clement Town Chowk to Turner Road Junction
    addRoad(&locality, "Clement Town Chowk", "Turner Road Junction", 0.30,
            false, 30.2655, 78.0010,
            false, 30.2675, 78.0035);

    // 3. Turner Road Junction to Velmed Hospital (Total ~1.30 km)
    addRoad(&locality, "Turner Road Junction", "Velmed Hospital (Turner Road)", 0.40,
            false, 30.2675, 78.0035,
            true,  30.2695, 78.0055);

    // 4. Turner Road Junction to Jeevan Jyoti Clinic (Total ~1.33 km)
    addRoad(&locality, "Turner Road Junction", "Jeevan Jyoti Clinic (Turner Road)", 0.43,
            false, 30.2675, 78.0035,
            true,  30.2700, 78.0060);

    // 5. Clement Town Chowk to ISBT / Haridwar Bypass Junction
    addRoad(&locality, "Clement Town Chowk", "ISBT / Haridwar Bypass Junction", 1.20,
            false, 30.2655, 78.0010,
            false, 30.2850, 77.9980);

    // 6. ISBT Junction to Bharat Heart Institute (Total ~2.61 km)
    addRoad(&locality, "ISBT / Haridwar Bypass Junction", "Bharat Heart Institute (Haridwar Bypass)", 0.81,
            false, 30.2850, 77.9980,
            true,  30.2910, 78.0040);

    // 7. Clement Town Chowk to Morowala Road
    addRoad(&locality, "Clement Town Chowk", "Morowala Chowk", 1.36,
            false, 30.2655, 78.0010,
            false, 30.2610, 78.0120);

    // 8. Morowala Chowk to Indresh Hospital (Total ~2.96 km)
    addRoad(&locality, "Morowala Chowk", "Indresh Hospital (Morowala)", 1.00,
            false, 30.2610, 78.0120,
            true,  30.2580, 78.0200);

    // 9. ISBT Junction to Prem Nagar (Chakrata Road route)
    addRoad(&locality, "ISBT / Haridwar Bypass Junction", "Prem Nagar (Chakrata Road)", 8.50,
            false, 30.2850, 77.9980,
            false, 30.3340, 77.9600);

    // 10. Prem Nagar to Graphic Era Hospital GEIMS, Dhulkot Mafi (Total ~21.0 km)
    addRoad(&locality, "Prem Nagar (Chakrata Road)", "Graphic Era Hospital (GEIMS, Dhulkot Mafi)", 10.70,
            false, 30.3340, 77.9600,
            true,  30.3540, 77.8820);

    // Run Dijkstra starting from Graphic Era University
    int start_idx = findOrAddNode(&locality, "Graphic Era University (Clement Town)", false, 30.2672, 77.9975);
    double dist[MAX_NODES];
    int parent[MAX_NODES];
    dijkstra(&locality, start_idx, dist, parent);

    // Collect all registered hospitals into a list for the user menu
    int hospital_indices[MAX_NODES];
    int hosp_count = 0;
    int closest_hosp_idx = -1;
    double min_dist = INF;

    for (int i = 0; i < locality.node_count; i++) {
        if (locality.nodes[i].is_hospital) {
            hospital_indices[hosp_count++] = i;
            if (dist[i] < min_dist) {
                min_dist = dist[i];
                closest_hosp_idx = i;
            }
        }
    }

    // --- Interactive User Menu ---
    int choice;
    while (1) {
        printf("\n=================================================================\n");
        printf("🏥 UNI-CARE EMERGENCY HOSPITAL SELECTOR (Origin: Graphic Era Univ)\n");
        printf("=================================================================\n");
        printf("0. 🚨 AUTO-SELECT NEAREST HOSPITAL (Recommended for Emergency)\n");
        for (int i = 0; i < hosp_count; i++) {
            int h_idx = hospital_indices[i];
            printf("%d. %s (%.2f km away)\n", i + 1, locality.nodes[h_idx].name, dist[h_idx]);
        }
        printf("%d. Exit Program\n", hosp_count + 1);
        printf("-----------------------------------------------------------------\n");
        printf("Enter your choice (0 - %d): ", hosp_count + 1);

        int scanRes = scanf("%d", &choice);
        if (scanRes == EOF) {
            break;
        }
        if (scanRes != 1) {
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF);
            printf("Invalid input! Please enter a number.\n");
            continue;
        }

        if (choice == 0) {
            displayRouteToHospital(&locality, start_idx, closest_hosp_idx, dist, parent);
        } else if (choice >= 1 && choice <= hosp_count) {
            int selected_hosp_idx = hospital_indices[choice - 1];
            displayRouteToHospital(&locality, start_idx, selected_hosp_idx, dist, parent);
        } else if (choice == hosp_count + 1) {
            printf("Exiting Uni-Care System. Stay safe!\n");
            break;
        } else {
            printf("Invalid option! Please choose between 0 and %d.\n", hosp_count + 1);
        }
    }

    return 0;
}