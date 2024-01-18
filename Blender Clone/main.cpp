#include <cstddef>
#include <cstdint>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <glbinding/Binding.h>
#include <glbinding/gl/gl.h>
#include <SFML/Window.hpp>


using namespace gl;

float maxValue = -100;


// Quelques couleurs en format RGB réel (0 à 1).
float grey[] = { 0.5f, 0.5f, 0.5f };
float white[] = { 1.0f, 1.0f, 1.0f };
float brightRed[] = { 1.0f, 0.2f, 0.2f };
float brightGreen[] = { 0.2f, 1.0f, 0.2f };
float brightBlue[] = { 0.2f, 0.2f, 1.0f };
float brightYellow[] = { 1.0f, 1.0f, 0.2f };

std::vector<std::vector<float>> colors = {
		{0.5f, 0.5f, 0.5f},     // grey
		{1.0f, 1.0f, 1.0f},     // white
		//{1.0f, 0.2f, 0.2f},     // brightRed
		//{0.2f, 1.0f, 0.2f},     // brightGreen
		//{0.2f, 0.2f, 1.0f},     // brightBlue
		//{1.0f, 1.0f, 0.2f}      // brightYellow
};

struct Vertex {
	float x, y, z;
};

struct Face {
	int v1, v2, v3;
};

void drawTriangle(Vertex v1, Vertex v2, Vertex v3, std::vector<float>& color) {
	glColor3fv(color.data());
	glBegin(GL_TRIANGLES); {
		glVertex3f(v1.x / maxValue, v1.y / maxValue, v1.z / maxValue);
		glVertex3f(v2.x / maxValue, v2.y / maxValue, v2.z / maxValue);
		glVertex3f(v3.x / maxValue, v3.y / maxValue, v3.z / maxValue);
	} glEnd();
}


void drawObject(std::vector<Face> faces, std::vector<Vertex> vertices) {
	int index = 0;
	for (Face face : faces) {
		drawTriangle(vertices[face.v1 - 1], vertices[face.v2 - 1], vertices[face.v3 - 1], colors[index % 2]);
		index++;
	}
}


std::vector<Vertex> parseFile(std::string filename, std::vector<Face>& faces) {
	std::vector<Vertex> vertices;

	std::ifstream file(filename);
	if (!file.is_open()) {
		std::cerr << "Error opening file: " << filename << std::endl;
		return vertices; 
	}

	std::string line;
	while (std::getline(file, line)) {
		std::istringstream iss(line);
		std::string type;
		iss >> type;

		if (type == "v") {
			Vertex v;
			iss >> v.x >> v.y >> v.z;
			maxValue = std::max((float)std::max(std::max(v.x, v.y), v.z), maxValue);

			vertices.push_back(v);
		}
		else if (type == "f") {
			Face f1;
			Face f2;
			std::vector<int> indices;
			std::string token;

			while (iss >> token) {
				size_t pos = token.find('/');
				if (pos != std::string::npos) {
					int index = std::stoi(token.substr(0, pos));
					indices.push_back(index);
				}
			}

			for (int i = 1; i < indices.size() - 1; i++) {
				f1.v1 = indices[0]; 
				f1.v2 = indices[i]; 
				f1.v3 = indices[i + 1];
				faces.push_back(f1);
			}
		}
	}

	file.close();
	return vertices;
}

int main(int argc, char* argv[]) {
	std::string filename = "..\\3d models\\chair_01.obj";
	std::vector<Face> faces;
	std::vector<Vertex> objVertices = parseFile(filename, faces);

	/*for (const auto& vertex : objVertices) {
		std::cout << "Vertex: (" << vertex.x << ", " << vertex.y << ", " << vertex.z << ")\n";
	}

	for (const auto& face : faces) {
		std::cout << "FACE: (" << face.v1 << ", " << face.v2 << ", " << face.v3 << ")\n";
	}*/

	std::cout << maxValue;



	sf::Window window;
	window.create(
		{ 800, 800 },            
		"Blender Clone",		
		sf::Style::Default,     
		sf::ContextSettings(24)
	);
	window.setFramerateLimit(30);
	window.setActive(true);
	glbinding::Binding::initialize(nullptr, true); 

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-1.0, 1.0, -1.0, 1.0, 3, 10);
	glTranslatef(0, 0, -6);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glEnable(GL_DEPTH_TEST);

	glClearColor(0.05f, 0.1f, 0.1f, 1.0f);

	glEnable(GL_LINE_SMOOTH);
	glLineWidth(5.0f);
	glEnable(GL_POINT_SMOOTH);
	glPointSize(5.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// Boucle d'exécution

	double lastTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	int nbFrames = 0;

	while (window.isOpen()) {

		double currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		nbFrames++;

		if (currentTime - lastTime >= 1000.0) { 
			// printf and reset timer
			std::cout << 1000.0 / static_cast<double>(nbFrames) << " ms/frame\n";
			nbFrames = 0;
			lastTime += 1000.0;
		}


		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glRotatef(1, 0, 1, 0);

		// Commentez/décommentez chacune des lignes pour tester chaque affichage.
		// drawWhiteTriangle();
		// drawColoredTriangle();
		// drawPyramid();
		drawObject(faces, objVertices); 

		window.display();
		sf::Event e;
		while (window.pollEvent(e)) {
			if (e.type == sf::Event::EventType::Closed)
				window.close();
		}
	}
	

	return 0;
}

