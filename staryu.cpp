/**
 * Program Name: Staryu
 * Author: William Kendrick Coleman
 * Description: Staryu takes in the number of credits you have earned for a given academic major/area of interest and creates a star/spider/radar chart, displaying where your strengths lie in the eyes of academic nuts. 
 */

#include <iostream> // for general input and output
#include <fstream> // for reading and writing to files
#include <vector> // for creating vectors
#include <string> // for string functions
#include <sstream> // for reading string from text file
#include <cmath> // for most math operations

using namespace std; // standard namespace (io, vetcors, etc)

const double PI = 3.14159265358979323846; // set static PI value

// Data Structures

// Coordinate Point
struct Point {
    double x;
    double y;
};

// Major Statistics
struct stat {
    string interest;
    int credits;
};


/**
 * @brief Returns coordinate for a given input
 * 
 * @param num The input value that determines distance
 * @param angle The angle for the point to be attributed towards
 * @param center The center of the star chart's circle
 * @param radius The radius of the star chart's circle
 * @param maxValue The max possible point value
 * 
 * @return The x-value and y-value of coordinate
 */
Point getCoordinates(double num, double angle, double center, double radius, double maxValue) {
    Point coord;
    coord.x = center + (radius * (num/maxValue)) * cos(angle);
    coord.y = center + (radius * (num/maxValue)) * sin(angle);
    return coord;
}

int main () {

    // Pulling in interests and their stats
    ifstream statsFile("academia.txt"); // input file stream
    vector<stat> stats;
    string fileStat;

    // error case
    if (!statsFile.is_open()) {cerr << "Could not open file" << endl; return 1;} 

    // Pull stats from file
    while (getline(statsFile, fileStat)) {
        stringstream ss(fileStat);
        string label, numString;

        // Interest left of ":"
        if (getline(ss, label, ':')) {
            // Get credits after ":"
            if (getline(ss, numString)) {
                // Input stat, converting numString to integer
                stats.push_back({label, stoi(numString)});
            }
        }
    }
    statsFile.close();

    // Chart configuration
    string filename = "academia.svg";
    int size = 400;
    double maxVal = 55.0;
    double center = size / 2.0;
    double radius = (size / 2.0) * 0.7;
    int n = stats.size();

    ofstream svg(filename); // output file stream

    // SVG Header & CSS styles
    svg << "<svg width=\"" << size + 200 << "\"" << " height=\"" << size << "\"" << " viewBox=\"0 0 " << size + 100 << " " << size << "\" "
        << "preserveAspectRatio=\"xMidYmid meet\" xmlns=\"http://www.w3.org/2000/svg\">\n";
    svg << "<style>\n"
        << "    @keyframes grow { from { transform: scale(0); opacity: 0; } to { transform: scale(1); opacity: 1; } }\n"
        << "    @keyframes fadeIn { from {transform: scale(0); opacity(0); } to { opacity: 1;} \n}"
        << "    .grid-ring { animation: fadeIn 0.5s ease-in forwards; opacity: 0; }\n"
        << "    .ring-1 { animation-delay: 0.1s; }\n"
        << "    .ring-2 { animation-delay: 0.2s; }\n"
        << "    .ring-3 { animation-delay: 0.3s; }\n"
        << "    .ring-4 { animation-delay: 0.4s; }\n"
        << "    .ring-5 { animation-delay: 0.5s; }\n"
        << "    .star-shape { transform-origin: " << center << "px " << center << "px; animation: grow 1.5s ease-out 0.7s both; }\n"
        << "    .animate-in { animation: fadeIn 0.5s ease-in both }\n"
        << "    .delay-1 { animation-delay: 0.6s; }\n"
        << "    .delay-2 { animation-delay: 0.3}\n"
        << "    .point { fill: #0933da00; cursor: pointer; }\n"
        << "    .point:hover + .tooltip { opacity: 1; }\n"
        << "    .tooltip {opacity: 0; transition: opacity 0.2s; pointer-events: none; }\n"
        << "</style>\n";

    // Background
    svg << "<rect width=\"100%\" height=\"100%\" fill=\"#0d1117\" rx=\"10\"/>\n";

    // Shifting for placement
    svg << "<g transform=\"translate(50, 0)\">\n";

    // Grid Rings (5 rings chosen)
    for (int i=1; i <= 5; ++i) {
        svg << "<polygon class=\"grid-ring ring-" << i << "\" points=\"";
        for (int j=0; j < n; ++j) {
            double angle = j * (2 * PI / n) - PI / 2;
            Point p = getCoordinates(maxVal * (i/5.0), angle, center, radius, maxVal);
            svg << p.x << "," << p.y << " ";
        }
        svg << "\" fill=\"none\" stroke=\"#ffffff\" />\n";
    }

    // Axis and Labels
    for (int i=0; i < n; ++i) {
        double angle = i * (2 * PI / n) - PI / 2;
        Point p = getCoordinates(maxVal, angle, center, radius, maxVal);
        Point lp = getCoordinates(maxVal * 1.15, angle, center, radius, maxVal);
        svg << "<line x1=\"" << center << "\" y1=\"" << center << "\" x2=\"" << p.x << "\" y2=\"" << p.y << "\" stroke=\"#ffffff\" class=\"animate-in delay-2\"/>\n";
        svg << "<text x=\"" << lp.x << "\" y=\"" << lp.y << "\" text-anchor=\"middle\" font-family=\"sans-serif\" font-size=\"12\" fill=\"#ffffff\" class=\"animate-in delay-1\">" << stats[i].interest << "</text>\n";
    }

    // Star Shape
    svg << "<polygon class=\"star-shape\" points=\"";
    for (int i=0; i < n; ++i) {
        double angle = i * (2 * PI / n) - PI / 2;
        Point p = getCoordinates(stats[i].credits, angle, center, radius, maxVal);
        svg << p.x << "," << p.y << " ";
    }
    svg << "\" fill=\"rgba(101, 112, 239, 0.93)\" stroke=\"#6e78ccc2\" stroke-width=\"2\" />\n";

    // Interactive Points & tool tips
    svg << "<g class=\"animate-in delay-1\">";
    for (int i=0; i < n; ++i) {
        double angle = i * (2 * PI / n) - PI / 2;
        Point p = getCoordinates(stats[i].credits, angle, center, radius, maxVal);

        // Point circle
        svg << "<circle class=\"point\" cx=\"" << p.x << "\" cy=\"" << p.y << "\" r=\"5\" />\n";

        // Tooltip (shown while hovering)
        svg << "<g class=\"tooltip\">\n"
            << " <rect x=\"" << p.x - 15 << "\" y=\"" << p.y - 30 << "\" width=\"50\" height=\"20\" rx=\"4\" fill=\"#24292f\" />\n"
            << " <text x=\"" << p.x + 10 << "\" y=\"" << p.y - 16 << "\" text-anchor=\"middle\" font-size=\"10\" fill=\"white\">" << stats[i].credits << " credits" << "</text>\n"
            << "</g>\n";
    }
    svg << "</g>\n"; // end animate-in delay-1

    svg << "</g>\n"; // end transform
    svg << "</svg>"; // end svg
    svg.close(); // close svg file

    cout << "Generated: " << filename << endl; // print output file name
    return 0;
}

