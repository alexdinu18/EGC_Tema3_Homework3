//-------------------------------------------------------------------------------------------------
// Descriere: fisier main
//
// Autor: student
// Data: today
//-------------------------------------------------------------------------------------------------

//incarcator de meshe
#include "lab_mesh_loader.hpp"

//geometrie: drawSolidCube, drawWireTeapot...
#include "lab_geometry.hpp"

//incarcator de shadere
#include "lab_shader_loader.hpp"

//interfata cu glut, ne ofera fereastra, input, context opengl
#include "lab_glut.hpp"

//camera
#include "lab_camera.hpp"

//time
#include <ctime>

float x_tr, y_tr, x_tr2, y_tr2, angle;
int timer = 1, cameras = 0;
bool first = true, earthquake = false;

struct VertexFormat{
	glm::vec3 pos;
	glm::vec3 c;

	VertexFormat (float x, float y, float z, int r, int g, int b){
		pos[0] = x;
		pos[1] = y;
		pos[2] = z;
		c[0] = r;
		c[1] = g;
		c[2] = b;
	}

};


class Scene : public lab::glut::WindowListener{

//variabile
private:
	lab::Camera cam;
	glm::mat4 original_model_matrix;
	glm::mat4 model_matrix, view_matrix, projection_matrix;								//matrici 4x4 pt modelare vizualizare proiectie
	unsigned int location_model_matrix, location_view_matrix, location_projection_matrix;//locatii la nivel de shader ale matricilor (un pointer/offset)
	unsigned int gl_program_shader;														//id-ul de opengl al obiectului de tip program shader

	//unsigned int mesh_vbo, mesh_ibo, mesh_vao, mesh_num_indices;						//containere opengl pentru vertecsi, indecsi si stare
	std::vector<unsigned int> mesh_vbo, mesh_ibo, mesh_vao, mesh_num_indices;
	unsigned int nr_elem;
	float x_player, y_player, z_player,x_player2, y_player2, z_player2;
	float x_FPOS1, y_FPOS1, z_FPOS1;
	float x_FPOS2, y_FPOS2, z_FPOS2;

//metode
public:
	
	//constructor .. e apelat cand e instantiata clasa
	Scene(){
		//setari pentru desenare, clear color seteaza culoarea de clear pentru ecran (format R,G,B,A)
		glClearColor(0.5,0.5,0.5,1);
		glClearDepth(1);			//clear depth si depth test (nu le studiem momentan, dar avem nevoie de ele!)
		glEnable(GL_DEPTH_TEST);	//sunt folosite pentru a determina obiectele cele mai apropiate de camera (la curs: algoritmul pictorului, algoritmul zbuffer)
		
		//incarca un shader din fisiere si gaseste locatiile matricilor relativ la programul creat
		gl_program_shader = lab::loadShader("shadere\\shader_vertex.glsl", "shadere\\shader_fragment.glsl");
		location_model_matrix = glGetUniformLocation(gl_program_shader, "model_matrix");
		location_view_matrix = glGetUniformLocation(gl_program_shader, "view_matrix");
		location_projection_matrix = glGetUniformLocation(gl_program_shader, "projection_matrix");
		
		nr_elem = 0;		
		x_tr = y_tr = angle = x_tr2 = y_tr2 = 0.0f;

		createMesheManualTrampoline2(-70.0, -30.0, -35.0);
		createMesheManualQuad(0.0, -30.0, 5.0);
		createMesheManualTrampoline1(-70.0, -30.0, -2.0);
		createMeshManualSecondPlayer(-80.0, 14.0, -35.0, true);
		createMesheFPOS1(-80.0, 17.0, -26.0, false);
		createMeshManualPlayer(-80.0, 4.0, -2.0, true);
		createMesheFPOS2(-80.0, 6.0, 5.0, true);

		//matrici de modelare si vizualizare
		original_model_matrix = glm::mat4(1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1);
		view_matrix = glm::lookAt(glm::vec3(0,0,50), glm::vec3(0,0,0), glm::vec3(0,1,0));
		cam.set(glm::vec3(0,0,10), glm::vec3(0,0,0), glm::vec3(0,1,0));

		//desenare wireframe
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	//destructor .. e apelat cand e distrusa clasa
	~Scene(){
		//distruge shader
		glDeleteProgram(gl_program_shader);

		//distruge mesh incarcat
		for (unsigned int vbo : mesh_vbo)
			glDeleteBuffers(1,&vbo);
		for (unsigned int ibo : mesh_ibo)
			glDeleteBuffers(1,&ibo);
		for (unsigned int vao : mesh_vao)
			glDeleteVertexArrays(1,&vao);
		nr_elem = 0;
	}


	//--------------------------------------------------------------------------------------------
	//functii de constructie geometrie------------------------------------------------------------
	
	void createMesheManualTrampoline1(float x, float y, float z){
		
		// (x, y, z) - starting 3D point

		std::vector<VertexFormat> vertexes;
		std::vector<glm::uvec3> indexes;
		
		// column 1
		for (int i = 0; i < 30; i += 10) {
			vertexes.push_back(VertexFormat(x,		y+i,		z,		0.6,0.6,0.6));
			vertexes.push_back(VertexFormat(x+2,	y+i,		z,		0.6,0.6,0.6)); 
			vertexes.push_back(VertexFormat(x,		y+10+i,		z,		0.6,0.6,0.6));
			vertexes.push_back(VertexFormat(x,		y+i,		z+2,	0.6,0.6,0.6));
			vertexes.push_back(VertexFormat(x+2,	y+10+i,		z,		0.6,0.6,0.6));
			vertexes.push_back(VertexFormat(x+2,	y+i,		z+2,	0.6,0.6,0.6));
			vertexes.push_back(VertexFormat(x,		y+10+i,		z+2,	0.6,0.6,0.6));
			vertexes.push_back(VertexFormat(x+2,	y+10+i,		z+2,	0.6,0.6,0.6));

			indexes.push_back(glm::uvec3(0,1,2));	
			indexes.push_back(glm::uvec3(2,4,1));
			indexes.push_back(glm::uvec3(4,2,7));	
			indexes.push_back(glm::uvec3(6,7,2));
			indexes.push_back(glm::uvec3(4,1,7));	
			indexes.push_back(glm::uvec3(5,1,7));
			indexes.push_back(glm::uvec3(0,2,6));	
			indexes.push_back(glm::uvec3(0,6,3));
			indexes.push_back(glm::uvec3(3,5,6));	
			indexes.push_back(glm::uvec3(6,5,7));
			indexes.push_back(glm::uvec3(3,0,5));	
			indexes.push_back(glm::uvec3(0,5,1));

			mesh_vao.push_back(0);
			glGenVertexArrays(1 , &mesh_vao[nr_elem]);
			glBindVertexArray(mesh_vao[nr_elem]);

			mesh_vbo.push_back(0);
			glGenBuffers(1, &mesh_vbo[nr_elem]);
			glBindBuffer(GL_ARRAY_BUFFER, mesh_vbo[nr_elem]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(VertexFormat)*vertexes.size(), &vertexes[0], GL_STATIC_DRAW);

			mesh_ibo.push_back(0);
			glGenBuffers(1, &mesh_ibo[nr_elem]);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_ibo[nr_elem]);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*indexes.size()*3, &indexes[0], GL_STATIC_DRAW);
		
			glEnableVertexAttribArray(0);	
			glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(VertexFormat),(void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(VertexFormat),(void*)(sizeof(glm::vec3)));

			mesh_num_indices.push_back(indexes.size()*3);
			nr_elem++;
			vertexes.clear();
			indexes.clear();
		
		}

		// column 2
		for (int i = 0; i < 20; i += 10) {
			vertexes.push_back(VertexFormat(x+50,		y+i,		z,		0.8,0.8,0.8));
			vertexes.push_back(VertexFormat(x+2+50,		y+i,		z,		0.8,0.8,0.8)); 
			vertexes.push_back(VertexFormat(x+50,		y+10+i,		z,		0.8,0.8,0.8));
			vertexes.push_back(VertexFormat(x+50,		y+i,		z+2,	0.8,0.8,0.8));
			vertexes.push_back(VertexFormat(x+2+50,		y+10+i,		z,		0.8,0.8,0.8));
			vertexes.push_back(VertexFormat(x+2+50,		y+i,		z+2,	0.8,0.8,0.8));
			vertexes.push_back(VertexFormat(x+50,		y+10+i,		z+2,	0.8,0.8,0.8));
			vertexes.push_back(VertexFormat(x+2+50,		y+10+i,		z+2,	0.8,0.8,0.8));

			indexes.push_back(glm::uvec3(0,1,2));	
			indexes.push_back(glm::uvec3(2,4,1));
			indexes.push_back(glm::uvec3(4,2,7));	
			indexes.push_back(glm::uvec3(6,7,2));
			indexes.push_back(glm::uvec3(4,1,7));	
			indexes.push_back(glm::uvec3(5,1,7));
			indexes.push_back(glm::uvec3(0,2,6));	
			indexes.push_back(glm::uvec3(0,6,3));
			indexes.push_back(glm::uvec3(3,5,6));	
			indexes.push_back(glm::uvec3(6,5,7));
			indexes.push_back(glm::uvec3(3,0,5));	
			indexes.push_back(glm::uvec3(0,5,1));

			mesh_vao.push_back(0);
			glGenVertexArrays(1 , &mesh_vao[nr_elem]);
			glBindVertexArray(mesh_vao[nr_elem]);

			mesh_vbo.push_back(0);
			glGenBuffers(1, &mesh_vbo[nr_elem]);
			glBindBuffer(GL_ARRAY_BUFFER, mesh_vbo[nr_elem]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(VertexFormat)*vertexes.size(), &vertexes[0], GL_STATIC_DRAW);

			mesh_ibo.push_back(0);
			glGenBuffers(1, &mesh_ibo[nr_elem]);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_ibo[nr_elem]);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*indexes.size()*3, &indexes[0], GL_STATIC_DRAW);
		
			glEnableVertexAttribArray(0);	
			glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(VertexFormat),(void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(VertexFormat),(void*)(sizeof(glm::vec3)));

			mesh_num_indices.push_back(indexes.size()*3);
			nr_elem++;
			vertexes.clear();
			indexes.clear();
		
		}
		
		// ramp, first piece
			vertexes.push_back(VertexFormat(x-10,		y+32,		z-6,		0.8,0.8,0.8));
			vertexes.push_back(VertexFormat(x/2-10,		y+25,		z-6,		0.8,0.8,0.8)); 
			vertexes.push_back(VertexFormat(x-10,		y+34,		z-6,		1,1,1));
			vertexes.push_back(VertexFormat(x-10,		y+32,		z+6,		0.8,0.8,0.8));
			vertexes.push_back(VertexFormat(x/2-10,		y+27,		z-6,		1,1,1));
			vertexes.push_back(VertexFormat(x/2-10,		y+25,		z+6,		0.8,0.8,0.8));
			vertexes.push_back(VertexFormat(x-10,		y+34,		z+6,		1,1,1));
			vertexes.push_back(VertexFormat(x/2-10,		y+27,		z+6,		1,1,1));

			indexes.push_back(glm::uvec3(0,1,2));	
			indexes.push_back(glm::uvec3(2,4,1));
			indexes.push_back(glm::uvec3(4,2,7));	
			indexes.push_back(glm::uvec3(6,7,2));
			indexes.push_back(glm::uvec3(4,1,7));	
			indexes.push_back(glm::uvec3(5,1,7));
			indexes.push_back(glm::uvec3(0,2,6));	
			indexes.push_back(glm::uvec3(0,6,3));
			indexes.push_back(glm::uvec3(3,5,6));	
			indexes.push_back(glm::uvec3(6,5,7));
			indexes.push_back(glm::uvec3(3,0,5));	
			indexes.push_back(glm::uvec3(0,5,1));

			mesh_vao.push_back(0);
			glGenVertexArrays(1 , &mesh_vao[nr_elem]);
			glBindVertexArray(mesh_vao[nr_elem]);

			mesh_vbo.push_back(0);
			glGenBuffers(1, &mesh_vbo[nr_elem]);
			glBindBuffer(GL_ARRAY_BUFFER, mesh_vbo[nr_elem]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(VertexFormat)*vertexes.size(), &vertexes[0], GL_STATIC_DRAW);

			mesh_ibo.push_back(0);
			glGenBuffers(1, &mesh_ibo[nr_elem]);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_ibo[nr_elem]);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*indexes.size()*3, &indexes[0], GL_STATIC_DRAW);
		
			glEnableVertexAttribArray(0);	
			glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(VertexFormat),(void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(VertexFormat),(void*)(sizeof(glm::vec3)));

			mesh_num_indices.push_back(indexes.size()*3);
			nr_elem++;
			vertexes.clear();
			indexes.clear();


		// ramp, second piece
			vertexes.push_back(VertexFormat(x/2-10,		y+25,		z-6,		0.8,0.8,0.8));
			vertexes.push_back(VertexFormat(x + 60,		y+18,		z-6,		0.8,0.8,0.8)); 
			vertexes.push_back(VertexFormat(x/2-10,		y+27,		z-6,		1,1,1));
			vertexes.push_back(VertexFormat(x/2-10,		y+25,		z+6,		0.8,0.8,0.8));
			vertexes.push_back(VertexFormat(x + 60,		y+20,		z-6,		1,1,1));
			vertexes.push_back(VertexFormat(x + 60,		y+18,		z+6,		0.8,0.8,0.8));
			vertexes.push_back(VertexFormat(x/2-10,		y+27,		z+6,		1,1,1));
			vertexes.push_back(VertexFormat(x + 60,		y+20,		z+6,		1,1,1));
		
			indexes.push_back(glm::uvec3(0,1,2));	
			indexes.push_back(glm::uvec3(2,4,1));
			indexes.push_back(glm::uvec3(4,2,7));	
			indexes.push_back(glm::uvec3(6,7,2));
			indexes.push_back(glm::uvec3(4,1,7));	
			indexes.push_back(glm::uvec3(5,1,7));
			indexes.push_back(glm::uvec3(0,2,6));	
			indexes.push_back(glm::uvec3(0,6,3));
			indexes.push_back(glm::uvec3(3,5,6));	
			indexes.push_back(glm::uvec3(6,5,7));
			indexes.push_back(glm::uvec3(3,0,5));	
			indexes.push_back(glm::uvec3(0,5,1));

			mesh_vao.push_back(0);
			glGenVertexArrays(1 , &mesh_vao[nr_elem]);
			glBindVertexArray(mesh_vao[nr_elem]);

			mesh_vbo.push_back(0);
			glGenBuffers(1, &mesh_vbo[nr_elem]);
			glBindBuffer(GL_ARRAY_BUFFER, mesh_vbo[nr_elem]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(VertexFormat)*vertexes.size(), &vertexes[0], GL_STATIC_DRAW);

			mesh_ibo.push_back(0);
			glGenBuffers(1, &mesh_ibo[nr_elem]);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_ibo[nr_elem]);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*indexes.size()*3, &indexes[0], GL_STATIC_DRAW);
		
			glEnableVertexAttribArray(0);	
			glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(VertexFormat),(void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(VertexFormat),(void*)(sizeof(glm::vec3)));

			mesh_num_indices.push_back(indexes.size()*3);
			nr_elem++;
			vertexes.clear();
			indexes.clear();

	}

	void createMesheManualTrampoline2(float x, float y, float z) {
		
		// (x, y, z) - starting 3D point

		std::vector<VertexFormat> vertexes;
		std::vector<glm::uvec3> indexes;
		
		// column 1
		for (int i = 0; i < 40; i += 10) {
			vertexes.push_back(VertexFormat(x,		y+i,		z,		0.8,0.8,0.8));
			vertexes.push_back(VertexFormat(x+2,	y+i,		z,		0.8,0.8,0.8)); 
			vertexes.push_back(VertexFormat(x,		y+10+i,		z,		0.8,0.8,0.8));
			vertexes.push_back(VertexFormat(x,		y+i,		z+2,	0.8,0.8,0.8));
			vertexes.push_back(VertexFormat(x+2,	y+10+i,		z,		0.8,0.8,0.8));
			vertexes.push_back(VertexFormat(x+2,	y+i,		z+2,	0.8,0.8,0.8));
			vertexes.push_back(VertexFormat(x,		y+10+i,		z+2,	0.8,0.8,0.8));
			vertexes.push_back(VertexFormat(x+2,	y+10+i,		z+2,	0.8,0.8,0.8));

			indexes.push_back(glm::uvec3(0,1,2));	
			indexes.push_back(glm::uvec3(2,4,1));
			indexes.push_back(glm::uvec3(4,2,7));	
			indexes.push_back(glm::uvec3(6,7,2));
			indexes.push_back(glm::uvec3(4,1,7));	
			indexes.push_back(glm::uvec3(5,1,7));
			indexes.push_back(glm::uvec3(0,2,6));	
			indexes.push_back(glm::uvec3(0,6,3));
			indexes.push_back(glm::uvec3(3,5,6));	
			indexes.push_back(glm::uvec3(6,5,7));
			indexes.push_back(glm::uvec3(3,0,5));	
			indexes.push_back(glm::uvec3(0,5,1));

			mesh_vao.push_back(0);
			glGenVertexArrays(1 , &mesh_vao[nr_elem]);
			glBindVertexArray(mesh_vao[nr_elem]);

			mesh_vbo.push_back(0);
			glGenBuffers(1, &mesh_vbo[nr_elem]);
			glBindBuffer(GL_ARRAY_BUFFER, mesh_vbo[nr_elem]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(VertexFormat)*vertexes.size(), &vertexes[0], GL_STATIC_DRAW);

			mesh_ibo.push_back(0);
			glGenBuffers(1, &mesh_ibo[nr_elem]);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_ibo[nr_elem]);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*indexes.size()*3, &indexes[0], GL_STATIC_DRAW);
		
			glEnableVertexAttribArray(0);	
			glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(VertexFormat),(void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(VertexFormat),(void*)(sizeof(glm::vec3)));

			mesh_num_indices.push_back(indexes.size()*3);
			nr_elem++;
			vertexes.clear();
			indexes.clear();
		
		}

		// column 2
		for (int i = 0; i < 30; i += 10) {
			vertexes.push_back(VertexFormat(x+50,		y+i,		z,		0.8,0.8,0.8));
			vertexes.push_back(VertexFormat(x+2+50,		y+i,		z,		0.8,0.8,0.8)); 
			vertexes.push_back(VertexFormat(x+50,		y+10+i,		z,		0.8,0.8,0.8));
			vertexes.push_back(VertexFormat(x+50,		y+i,		z+2,	0.8,0.8,0.8));
			vertexes.push_back(VertexFormat(x+2+50,		y+10+i,		z,		0.8,0.8,0.8));
			vertexes.push_back(VertexFormat(x+2+50,		y+i,		z+2,	0.8,0.8,0.8));
			vertexes.push_back(VertexFormat(x+50,		y+10+i,		z+2,	0.8,0.8,0.8));
			vertexes.push_back(VertexFormat(x+2+50,		y+10+i,		z+2,	0.8,0.8,0.8));

			indexes.push_back(glm::uvec3(0,1,2));	
			indexes.push_back(glm::uvec3(2,4,1));
			indexes.push_back(glm::uvec3(4,2,7));	
			indexes.push_back(glm::uvec3(6,7,2));
			indexes.push_back(glm::uvec3(4,1,7));	
			indexes.push_back(glm::uvec3(5,1,7));
			indexes.push_back(glm::uvec3(0,2,6));	
			indexes.push_back(glm::uvec3(0,6,3));
			indexes.push_back(glm::uvec3(3,5,6));	
			indexes.push_back(glm::uvec3(6,5,7));
			indexes.push_back(glm::uvec3(3,0,5));	
			indexes.push_back(glm::uvec3(0,5,1));

			mesh_vao.push_back(0);
			glGenVertexArrays(1 , &mesh_vao[nr_elem]);
			glBindVertexArray(mesh_vao[nr_elem]);

			mesh_vbo.push_back(0);
			glGenBuffers(1, &mesh_vbo[nr_elem]);
			glBindBuffer(GL_ARRAY_BUFFER, mesh_vbo[nr_elem]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(VertexFormat)*vertexes.size(), &vertexes[0], GL_STATIC_DRAW);

			mesh_ibo.push_back(0);
			glGenBuffers(1, &mesh_ibo[nr_elem]);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_ibo[nr_elem]);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*indexes.size()*3, &indexes[0], GL_STATIC_DRAW);
		
			glEnableVertexAttribArray(0);	
			glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(VertexFormat),(void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(VertexFormat),(void*)(sizeof(glm::vec3)));

			mesh_num_indices.push_back(indexes.size()*3);
			nr_elem++;
			vertexes.clear();
			indexes.clear();
		
		}
		
		// ramp, first piece
			vertexes.push_back(VertexFormat(x-10,		y+42,		z-6,		0.8,0.8,0.8));
			vertexes.push_back(VertexFormat(x/2-10,		y+35,		z-6,		0.8,0.8,0.8)); 
			vertexes.push_back(VertexFormat(x-10,		y+44,		z-6,		1,1,1));
			vertexes.push_back(VertexFormat(x-10,		y+42,		z+8,		0.8,0.8,0.8));
			vertexes.push_back(VertexFormat(x/2-10,		y+37,		z-6,		1,1,1));
			vertexes.push_back(VertexFormat(x/2-10,		y+35,		z+8,		0.8,0.8,0.8));
			vertexes.push_back(VertexFormat(x-10,		y+44,		z+8,		1,1,1));
			vertexes.push_back(VertexFormat(x/2-10,		y+37,		z+8,		1,1,1));

			indexes.push_back(glm::uvec3(0,1,2));	
			indexes.push_back(glm::uvec3(2,4,1));
			indexes.push_back(glm::uvec3(4,2,7));	
			indexes.push_back(glm::uvec3(6,7,2));
			indexes.push_back(glm::uvec3(4,1,7));	
			indexes.push_back(glm::uvec3(5,1,7));
			indexes.push_back(glm::uvec3(0,2,6));	
			indexes.push_back(glm::uvec3(0,6,3));
			indexes.push_back(glm::uvec3(3,5,6));	
			indexes.push_back(glm::uvec3(6,5,7));
			indexes.push_back(glm::uvec3(3,0,5));	
			indexes.push_back(glm::uvec3(0,5,1));

			mesh_vao.push_back(0);
			glGenVertexArrays(1 , &mesh_vao[nr_elem]);
			glBindVertexArray(mesh_vao[nr_elem]);

			mesh_vbo.push_back(0);
			glGenBuffers(1, &mesh_vbo[nr_elem]);
			glBindBuffer(GL_ARRAY_BUFFER, mesh_vbo[nr_elem]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(VertexFormat)*vertexes.size(), &vertexes[0], GL_STATIC_DRAW);

			mesh_ibo.push_back(0);
			glGenBuffers(1, &mesh_ibo[nr_elem]);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_ibo[nr_elem]);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*indexes.size()*3, &indexes[0], GL_STATIC_DRAW);
		
			glEnableVertexAttribArray(0);	
			glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(VertexFormat),(void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(VertexFormat),(void*)(sizeof(glm::vec3)));

			mesh_num_indices.push_back(indexes.size()*3);
			nr_elem++;
			vertexes.clear();
			indexes.clear();


		// ramp, second piece
			vertexes.push_back(VertexFormat(x/2-10,		y+35,		z-6,		0.8,0.8,0.8));
			vertexes.push_back(VertexFormat(x + 60,		y+28,		z-6,		0.8,0.8,0.8)); 
			vertexes.push_back(VertexFormat(x/2-10,		y+37,		z-6,		1,1,1));
			vertexes.push_back(VertexFormat(x/2-10,		y+35,		z+8,		0.8,0.8,0.8));
			vertexes.push_back(VertexFormat(x + 60,		y+30,		z-6,		1,1,1));
			vertexes.push_back(VertexFormat(x + 60,		y+28,		z+8,		0.8,0.8,0.8));
			vertexes.push_back(VertexFormat(x/2-10,		y+37,		z+8,		1,1,1));
			vertexes.push_back(VertexFormat(x + 60,		y+30,		z+8,		1,1,1));
		
			indexes.push_back(glm::uvec3(0,1,2));	
			indexes.push_back(glm::uvec3(2,4,1));
			indexes.push_back(glm::uvec3(4,2,7));	
			indexes.push_back(glm::uvec3(6,7,2));
			indexes.push_back(glm::uvec3(4,1,7));	
			indexes.push_back(glm::uvec3(5,1,7));
			indexes.push_back(glm::uvec3(0,2,6));	
			indexes.push_back(glm::uvec3(0,6,3));
			indexes.push_back(glm::uvec3(3,5,6));	
			indexes.push_back(glm::uvec3(6,5,7));
			indexes.push_back(glm::uvec3(3,0,5));	
			indexes.push_back(glm::uvec3(0,5,1));

			mesh_vao.push_back(0);
			glGenVertexArrays(1 , &mesh_vao[nr_elem]);
			glBindVertexArray(mesh_vao[nr_elem]);

			mesh_vbo.push_back(0);
			glGenBuffers(1, &mesh_vbo[nr_elem]);
			glBindBuffer(GL_ARRAY_BUFFER, mesh_vbo[nr_elem]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(VertexFormat)*vertexes.size(), &vertexes[0], GL_STATIC_DRAW);

			mesh_ibo.push_back(0);
			glGenBuffers(1, &mesh_ibo[nr_elem]);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_ibo[nr_elem]);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*indexes.size()*3, &indexes[0], GL_STATIC_DRAW);
		
			glEnableVertexAttribArray(0);	
			glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(VertexFormat),(void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(VertexFormat),(void*)(sizeof(glm::vec3)));

			mesh_num_indices.push_back(indexes.size()*3);
			nr_elem++;
			vertexes.clear();
			indexes.clear();

	}
	
	void createMesheManualQuad(float x, float y, float z) {
		std::vector<VertexFormat> vertexes;
		std::vector<glm::uvec3> indexes;
		
		for ( int i = 0; i <= 150; i += 10) {
			for ( int j = 30; j >= -80; j -= 10) {
			vertexes.push_back(VertexFormat(x+i,		y,		z+j,		0,1,0.05));
			vertexes.push_back(VertexFormat(x+i,		y,		z+j+10,		0,1,0.05));
			vertexes.push_back(VertexFormat(x+i+10,		y,		z+j+10,		0,1,0.05));
			vertexes.push_back(VertexFormat(x+i+10,		y,		z+j,		0,1,0.05)); 

			indexes.push_back(glm::uvec3(0,1,2));	
			indexes.push_back(glm::uvec3(2,0,3));

			mesh_vao.push_back(0);
			glGenVertexArrays(1 , &mesh_vao[nr_elem]);
			glBindVertexArray(mesh_vao[nr_elem]);

			mesh_vbo.push_back(0);
			glGenBuffers(1, &mesh_vbo[nr_elem]);
			glBindBuffer(GL_ARRAY_BUFFER, mesh_vbo[nr_elem]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(VertexFormat)*vertexes.size(), &vertexes[0], GL_STATIC_DRAW);

			mesh_ibo.push_back(0);
			glGenBuffers(1, &mesh_ibo[nr_elem]);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_ibo[nr_elem]);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*indexes.size()*3, &indexes[0], GL_STATIC_DRAW);
		
			glEnableVertexAttribArray(0);	
			glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(VertexFormat),(void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(VertexFormat),(void*)(sizeof(glm::vec3)));

			mesh_num_indices.push_back(indexes.size()*3);
			nr_elem++;
			vertexes.clear();
			indexes.clear();
			}
		}
	}

	void createMeshManualPlayer(float x, float y, float z, bool tilt) {
		
		std::vector<VertexFormat> vertexes;
		std::vector<glm::uvec3> indexes;
		int length = 4;
		
		if (tilt) {
			vertexes.push_back(VertexFormat(x,			y,						z-length/2,	0,0,1));
			vertexes.push_back(VertexFormat(x+length,	y-0.2*length,			z-length/2,	0,0,1)); 
			vertexes.push_back(VertexFormat(x,			y+length,				z-length/2,	0,0,1));
			vertexes.push_back(VertexFormat(x,			y,						z+length/2,	0,0,1));
			vertexes.push_back(VertexFormat(x+length,	y+length-0.2*length,	z-length/2,	0,0,1));
			vertexes.push_back(VertexFormat(x+length,	y-0.2*length,			z+length/2,	0,0,1));
			vertexes.push_back(VertexFormat(x,			y+length,				z+length/2,	0,0,1));
			vertexes.push_back(VertexFormat(x+length,	y+length-0.2*length,	z+length/2,	0,0,1));
		}
		else {
			vertexes.push_back(VertexFormat(x,			y,			z-length/2,	0,0,1));
			vertexes.push_back(VertexFormat(x+length,	y,			z-length/2,	0,0,1)); 
			vertexes.push_back(VertexFormat(x,			y+length,	z-length/2,	0,0,1));
			vertexes.push_back(VertexFormat(x,			y,			z+length/2,	0,0,1));
			vertexes.push_back(VertexFormat(x+length,	y+length,	z-length/2,	0,0,1));
			vertexes.push_back(VertexFormat(x+length,	y,			z+length/2,	0,0,1));
			vertexes.push_back(VertexFormat(x,			y+length,	z+length/2,	0,0,1));
			vertexes.push_back(VertexFormat(x+length,	y+length,	z+length/2,	0,0,1));
		}

		indexes.push_back(glm::uvec3(0,1,2));
		indexes.push_back(glm::uvec3(2,4,1));
		indexes.push_back(glm::uvec3(4,2,7));	
		indexes.push_back(glm::uvec3(6,7,2));
		indexes.push_back(glm::uvec3(4,1,7));	
		indexes.push_back(glm::uvec3(5,1,7));
		indexes.push_back(glm::uvec3(0,2,6));	
		indexes.push_back(glm::uvec3(0,6,3));
		indexes.push_back(glm::uvec3(3,5,6));	
		indexes.push_back(glm::uvec3(6,5,7));
		indexes.push_back(glm::uvec3(3,0,5));	
		indexes.push_back(glm::uvec3(0,5,1));

		mesh_vao.push_back(0);
		glGenVertexArrays(1 , &mesh_vao[nr_elem]);
		glBindVertexArray(mesh_vao[nr_elem]);

		mesh_vbo.push_back(0);
		glGenBuffers(1, &mesh_vbo[nr_elem]);
		glBindBuffer(GL_ARRAY_BUFFER, mesh_vbo[nr_elem]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(VertexFormat)*vertexes.size(), &vertexes[0], GL_STATIC_DRAW);

		mesh_ibo.push_back(0);
		glGenBuffers(1, &mesh_ibo[nr_elem]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_ibo[nr_elem]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*indexes.size()*3, &indexes[0], GL_STATIC_DRAW);
		
		glEnableVertexAttribArray(0);	
		glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(VertexFormat),(void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(VertexFormat),(void*)(sizeof(glm::vec3)));

		mesh_num_indices.push_back(indexes.size()*3);
		nr_elem++;
		vertexes.clear();
		indexes.clear();
		
		x_player = x;
		y_player = y;
		z_player = z - length/2;
	}

	void createMeshManualSecondPlayer(float x, float y, float z, bool tilt) {
		
		std::vector<VertexFormat> vertexes;
		std::vector<glm::uvec3> indexes;
		int length = 6;
		
		if (tilt) {
			vertexes.push_back(VertexFormat(x,			y,						z-length/2+1,	0,1,1));
			vertexes.push_back(VertexFormat(x+length,	y-0.2*length,			z-length/2+1,	0,1,1)); 
			vertexes.push_back(VertexFormat(x,			y+length,				z-length/2+1,	0,1,1));
			vertexes.push_back(VertexFormat(x,			y,						z+length/2+1,	0,1,1));
			vertexes.push_back(VertexFormat(x+length,	y+length-0.2*length,	z-length/2+1,	0,1,1));
			vertexes.push_back(VertexFormat(x+length,	y-0.2*length,			z+length/2+1,	0,1,1));
			vertexes.push_back(VertexFormat(x,			y+length,				z+length/2+1,	0,1,1));
			vertexes.push_back(VertexFormat(x+length,	y+length-0.2*length,	z+length/2+1,	0,1,1));
		}
		else {
			vertexes.push_back(VertexFormat(x,			y,			z-length/2+1,	0,1,1));
			vertexes.push_back(VertexFormat(x+length,	y,			z-length/2+1,	0,1,1)); 
			vertexes.push_back(VertexFormat(x,			y+length,	z-length/2+1,	0,1,1));
			vertexes.push_back(VertexFormat(x,			y,			z+length/2+1,	0,1,1));
			vertexes.push_back(VertexFormat(x+length,	y+length,	z-length/2+1,	0,1,1));
			vertexes.push_back(VertexFormat(x+length,	y,			z+length/2+1,	0,1,1));
			vertexes.push_back(VertexFormat(x,			y+length,	z+length/2+1,	0,1,1));
			vertexes.push_back(VertexFormat(x+length,	y+length,	z+length/2+1,	0,1,1));
		}

		indexes.push_back(glm::uvec3(0,1,2));
		indexes.push_back(glm::uvec3(2,4,1));
		indexes.push_back(glm::uvec3(4,2,7));	
		indexes.push_back(glm::uvec3(6,7,2));
		indexes.push_back(glm::uvec3(4,1,7));	
		indexes.push_back(glm::uvec3(5,1,7));
		indexes.push_back(glm::uvec3(0,2,6));	
		indexes.push_back(glm::uvec3(0,6,3));
		indexes.push_back(glm::uvec3(3,5,6));	
		indexes.push_back(glm::uvec3(6,5,7));
		indexes.push_back(glm::uvec3(3,0,5));	
		indexes.push_back(glm::uvec3(0,5,1));

		mesh_vao.push_back(0);
		glGenVertexArrays(1 , &mesh_vao[nr_elem]);
		glBindVertexArray(mesh_vao[nr_elem]);

		mesh_vbo.push_back(0);
		glGenBuffers(1, &mesh_vbo[nr_elem]);
		glBindBuffer(GL_ARRAY_BUFFER, mesh_vbo[nr_elem]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(VertexFormat)*vertexes.size(), &vertexes[0], GL_STATIC_DRAW);

		mesh_ibo.push_back(0);
		glGenBuffers(1, &mesh_ibo[nr_elem]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_ibo[nr_elem]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*indexes.size()*3, &indexes[0], GL_STATIC_DRAW);
		
		glEnableVertexAttribArray(0);	
		glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(VertexFormat),(void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(VertexFormat),(void*)(sizeof(glm::vec3)));

		mesh_num_indices.push_back(indexes.size()*3);
		nr_elem++;
		vertexes.clear();
		indexes.clear();

		x_player2 = x;
		y_player2 = y;
		z_player2 = z - length/2;
	}

	void createMesheFPOS1(float x, float y, float z, bool tilt) {
		std::vector<VertexFormat> vertexes;
		std::vector<glm::uvec3> indexes;
		int length = 2;
		
		if (tilt) {
			vertexes.push_back(VertexFormat(x,			y,						z-length/2,	0,0,0));
			vertexes.push_back(VertexFormat(x+length,	y-0.2*length,			z-length/2,	0,0,0)); 
			vertexes.push_back(VertexFormat(x,			y+length,				z-length/2,	0,0,0));
			vertexes.push_back(VertexFormat(x,			y,						z+length/2,	0,0,0));
			vertexes.push_back(VertexFormat(x+length,	y+length-0.2*length,	z-length/2,	0,0,0));
			vertexes.push_back(VertexFormat(x+length,	y-0.2*length,			z+length/2,	0,0,0));
			vertexes.push_back(VertexFormat(x,			y+length,				z+length/2,	0,0,0));
			vertexes.push_back(VertexFormat(x+length,	y+length-0.2*length,	z+length/2,	0,0,0));
		}
		else {
			vertexes.push_back(VertexFormat(x,			y,			z-length/2,	0,0,0));
			vertexes.push_back(VertexFormat(x+length,	y,			z-length/2,	0,0,0)); 
			vertexes.push_back(VertexFormat(x,			y+length,	z-length/2,	0,0,0));
			vertexes.push_back(VertexFormat(x,			y,			z+length/2,	0,0,0));
			vertexes.push_back(VertexFormat(x+length,	y+length,	z-length/2,	0,0,0));
			vertexes.push_back(VertexFormat(x+length,	y,			z+length/2,	0,0,0));
			vertexes.push_back(VertexFormat(x,			y+length,	z+length/2,	0,0,0));
			vertexes.push_back(VertexFormat(x+length,	y+length,	z+length/2,	0,0,0));
		}

		indexes.push_back(glm::uvec3(0,1,2));
		indexes.push_back(glm::uvec3(2,4,1));
		indexes.push_back(glm::uvec3(4,2,7));	
		indexes.push_back(glm::uvec3(6,7,2));
		indexes.push_back(glm::uvec3(4,1,7));	
		indexes.push_back(glm::uvec3(5,1,7));
		indexes.push_back(glm::uvec3(0,2,6));	
		indexes.push_back(glm::uvec3(0,6,3));
		indexes.push_back(glm::uvec3(3,5,6));	
		indexes.push_back(glm::uvec3(6,5,7));
		indexes.push_back(glm::uvec3(3,0,5));	
		indexes.push_back(glm::uvec3(0,5,1));

		mesh_vao.push_back(0);
		glGenVertexArrays(1 , &mesh_vao[nr_elem]);
		glBindVertexArray(mesh_vao[nr_elem]);

		mesh_vbo.push_back(0);
		glGenBuffers(1, &mesh_vbo[nr_elem]);
		glBindBuffer(GL_ARRAY_BUFFER, mesh_vbo[nr_elem]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(VertexFormat)*vertexes.size(), &vertexes[0], GL_STATIC_DRAW);

		mesh_ibo.push_back(0);
		glGenBuffers(1, &mesh_ibo[nr_elem]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_ibo[nr_elem]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*indexes.size()*3, &indexes[0], GL_STATIC_DRAW);
		
		glEnableVertexAttribArray(0);	
		glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(VertexFormat),(void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(VertexFormat),(void*)(sizeof(glm::vec3)));

		mesh_num_indices.push_back(indexes.size()*3);
		nr_elem++;
		vertexes.clear();
		indexes.clear();

		x_FPOS1 = x;
		y_FPOS1 = y;
		z_FPOS1 = z - length/2;
	}

	void createMesheFPOS2(float x, float y, float z, bool tilt) {
		std::vector<VertexFormat> vertexes;
		std::vector<glm::uvec3> indexes;
		int length = 2;
		
		if (tilt) {
			vertexes.push_back(VertexFormat(x,			y,						z-length/2,	0,0,0));
			vertexes.push_back(VertexFormat(x+length,	y-0.2*length,			z-length/2,	0,0,0)); 
			vertexes.push_back(VertexFormat(x,			y+length,				z-length/2,	0,0,0));
			vertexes.push_back(VertexFormat(x,			y,						z+length/2,	0,0,0));
			vertexes.push_back(VertexFormat(x+length,	y+length-0.2*length,	z-length/2,	0,0,0));
			vertexes.push_back(VertexFormat(x+length,	y-0.2*length,			z+length/2,	0,0,0));
			vertexes.push_back(VertexFormat(x,			y+length,				z+length/2,	0,0,0));
			vertexes.push_back(VertexFormat(x+length,	y+length-0.2*length,	z+length/2,	0,0,0));
		}
		else {
			vertexes.push_back(VertexFormat(x,			y,			z-length/2,	0,0,0));
			vertexes.push_back(VertexFormat(x+length,	y,			z-length/2,	0,0,0)); 
			vertexes.push_back(VertexFormat(x,			y+length,	z-length/2,	0,0,0));
			vertexes.push_back(VertexFormat(x,			y,			z+length/2,	0,0,0));
			vertexes.push_back(VertexFormat(x+length,	y+length,	z-length/2,	0,0,0));
			vertexes.push_back(VertexFormat(x+length,	y,			z+length/2,	0,0,0));
			vertexes.push_back(VertexFormat(x,			y+length,	z+length/2,	0,0,0));
			vertexes.push_back(VertexFormat(x+length,	y+length,	z+length/2,	0,0,0));
		}

		indexes.push_back(glm::uvec3(0,1,2));
		indexes.push_back(glm::uvec3(2,4,1));
		indexes.push_back(glm::uvec3(4,2,7));	
		indexes.push_back(glm::uvec3(6,7,2));
		indexes.push_back(glm::uvec3(4,1,7));	
		indexes.push_back(glm::uvec3(5,1,7));
		indexes.push_back(glm::uvec3(0,2,6));	
		indexes.push_back(glm::uvec3(0,6,3));
		indexes.push_back(glm::uvec3(3,5,6));	
		indexes.push_back(glm::uvec3(6,5,7));
		indexes.push_back(glm::uvec3(3,0,5));	
		indexes.push_back(glm::uvec3(0,5,1));

		mesh_vao.push_back(0);
		glGenVertexArrays(1 , &mesh_vao[nr_elem]);
		glBindVertexArray(mesh_vao[nr_elem]);

		mesh_vbo.push_back(0);
		glGenBuffers(1, &mesh_vbo[nr_elem]);
		glBindBuffer(GL_ARRAY_BUFFER, mesh_vbo[nr_elem]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(VertexFormat)*vertexes.size(), &vertexes[0], GL_STATIC_DRAW);

		mesh_ibo.push_back(0);
		glGenBuffers(1, &mesh_ibo[nr_elem]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_ibo[nr_elem]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*indexes.size()*3, &indexes[0], GL_STATIC_DRAW);
		
		glEnableVertexAttribArray(0);	
		glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(VertexFormat),(void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(VertexFormat),(void*)(sizeof(glm::vec3)));

		mesh_num_indices.push_back(indexes.size()*3);
		nr_elem++;
		vertexes.clear();
		indexes.clear();

		x_FPOS2 = x;
		y_FPOS2 = y;
		z_FPOS2 = z - length/2;
	}
	//--------------------------------------------------------------------------------------------
	//functii de cadru ---------------------------------------------------------------------------

	//functie chemata inainte de a incepe cadrul de desenare, o folosim ca sa updatam situatia scenei ( modelam/simulam scena)
	void notifyBeginFrame(){	}
	//functia de afisare (lucram cu banda grafica)
	void notifyDisplayFrame(){
		// camera FP de sus
		if (cameras % 7 == 0) {
			if (first)
				cam.set(glm::vec3(0,50,5), glm::vec3(0, 0, 0), glm::vec3(0,1,0));
			first = false;
		}
		// camera statică pt player1
		if (cameras % 7 == 1) {
			first = true;
			cam.staticFirstPlayerCamera();
		}
		// camera statică pt player2
		if (cameras % 7 == 2) {			
			cam.staticSecondPlayerCamera();
		}
		// camera FP pt player1
		if (cameras % 7 == 3) {
			if (earthquake) {
				glm::vec3 pos = glm::vec3(3*fabs(cos(timer - 100))+x_player + x_tr + 4, 3*fabs(cos(timer - 100))+y_player + y_tr + 2, cos(timer - 100)+z_player);
				glm::vec3 fwd = glm::vec3(x_player + x_tr + 4+200, y_player + y_tr + 2, z_player);
				cam.dynamicFirstPlayerCamera(pos, fwd);
			}
			else {
				glm::vec3 pos = glm::vec3(x_player + x_tr + 4, y_player + y_tr + 2, z_player);
				glm::vec3 fwd = glm::vec3(x_player + x_tr + 4+200, y_player + y_tr + 2, z_player);
				cam.dynamicFirstPlayerCamera(pos, fwd);
			}
		}
		// camera TP pt player1
		if (cameras % 7 == 4) {
			glm::vec3 pos = glm::vec3(x_FPOS1 + x_tr + 1, y_FPOS1 + y_tr + 1, z_FPOS1);
			glm::vec3 fwd = glm::vec3(x_FPOS1 + x_tr + 1, y_player + y_tr + 1+3, z_FPOS1-2000);
			cam.TPSFirstPlayerCamera(pos, fwd);
		}
		// camera FP pt player2
		if (cameras % 7 == 5) {
			glm::vec3 pos = glm::vec3(x_player2 + x_tr2 + 6, y_player2 + y_tr2 + 2, z_player2+2);
			glm::vec3 fwd = glm::vec3(x_player2 + x_tr2 + 4+200, y_player2 + y_tr2 + 2, z_player2);
			cam.dynamicFirstPlayerCamera(pos, fwd);
		}
		// camera TP pt player2
		if (cameras % 7 == 6) {
			glm::vec3 pos = glm::vec3(x_FPOS2 + x_tr2 + 1, y_FPOS2 + y_tr2 + 1, z_FPOS2);
			glm::vec3 fwd = glm::vec3(x_FPOS2 + x_tr2 + 1, y_player2 + y_tr2 + 1+3, z_FPOS2-2000);
			cam.TPSFirstPlayerCamera(pos, fwd);
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//foloseste shaderul
		glUseProgram(gl_program_shader);

		//trimite variabile uniforme la shader
		glUniformMatrix4fv(glGetUniformLocation(gl_program_shader,"model_matrix"),1,false,glm::value_ptr(model_matrix));
		glm::mat4 view_matrix = cam.getViewMatrix();
		glUniformMatrix4fv(glGetUniformLocation(gl_program_shader,"view_matrix"),1,false,glm::value_ptr(view_matrix));
		glUniformMatrix4fv(glGetUniformLocation(gl_program_shader,"projection_matrix"),1,false,glm::value_ptr(projection_matrix));
		// colorarea dinamică a personajului principal
		glUniform1f(glGetUniformLocation(gl_program_shader,"timer"),fabs(cos(timer - 100)));
		glUniform1i(glGetUniformLocation(gl_program_shader,"dynamic"),0);
		timer++;

		//bind obiect
		for (unsigned int i = 0; i < mesh_vao.size() - 11; i++) {
			glBindVertexArray(mesh_vao[i]);
			glDrawElements(GL_TRIANGLES, mesh_num_indices[i], GL_UNSIGNED_INT, 0);
		}

		glm::mat4 temp = model_matrix;
		model_matrix = glm::translate(original_model_matrix, glm::vec3(x_tr,y_tr,0));
			
			
		glUniformMatrix4fv(location_model_matrix,1,false,glm::value_ptr(model_matrix));
		glBindVertexArray(mesh_vao[nr_elem - 1]);
		glDrawElements(GL_TRIANGLES, mesh_num_indices[nr_elem - 1], GL_UNSIGNED_INT, 0);
		
		// dacă este cutrumur, trimit la shader model_matrix modificat pt palyer1 + trambulină
		if (earthquake && x_tr+x_player < - 10) {
			model_matrix = temp;
			model_matrix = glm::translate(original_model_matrix, glm::vec3(x_tr+3*fabs(cos(timer - 100)),y_tr+3*fabs(cos(timer - 100)),cos(timer - 100)));
			glUniform1i(glGetUniformLocation(gl_program_shader,"dynamic"),1);
			glUniformMatrix4fv(location_model_matrix,1,false,glm::value_ptr(model_matrix));
			glBindVertexArray(mesh_vao[nr_elem - 2]);
			glDrawElements(GL_TRIANGLES, mesh_num_indices[nr_elem - 2], GL_UNSIGNED_INT, 0);

			model_matrix = temp;
			model_matrix = glm::translate(original_model_matrix, glm::vec3(3*fabs(cos(timer - 100)),3*fabs(cos(timer - 100)),cos(timer - 100)));
			for (int i = 5; i < 12; i++) {
				glUniform1i(glGetUniformLocation(gl_program_shader,"dynamic"),0);
				glUniformMatrix4fv(location_model_matrix,1,false,glm::value_ptr(model_matrix));
				glBindVertexArray(mesh_vao[nr_elem - i]);
				glDrawElements(GL_TRIANGLES, mesh_num_indices[nr_elem - i], GL_UNSIGNED_INT, 0);
			}
		}
		// dacă nu e, trimit normal
		else {
			glUniform1i(glGetUniformLocation(gl_program_shader,"dynamic"),1);
			glUniformMatrix4fv(location_model_matrix,1,false,glm::value_ptr(model_matrix));
			glBindVertexArray(mesh_vao[nr_elem - 2]);
			glDrawElements(GL_TRIANGLES, mesh_num_indices[nr_elem - 2], GL_UNSIGNED_INT, 0);
			model_matrix = temp;
			for (int i = 5; i < 12; i++) {
				glUniform1i(glGetUniformLocation(gl_program_shader,"dynamic"),0);
				glUniformMatrix4fv(location_model_matrix,1,false,glm::value_ptr(model_matrix));
				glBindVertexArray(mesh_vao[nr_elem - i]);
				glDrawElements(GL_TRIANGLES, mesh_num_indices[nr_elem - i], GL_UNSIGNED_INT, 0);
			}
		}
			
		model_matrix = temp;
		
		// tanslație pt player2 și camera TP pt player2
		glUniform1i(glGetUniformLocation(gl_program_shader,"dynamic"),0);
		model_matrix = glm::translate(original_model_matrix, glm::vec3(x_tr2,y_tr2,0));

		glUniformMatrix4fv(location_model_matrix,1,false,glm::value_ptr(model_matrix));
		glBindVertexArray(mesh_vao[nr_elem - 3]);
		glDrawElements(GL_TRIANGLES, mesh_num_indices[nr_elem - 3], GL_UNSIGNED_INT, 0);

		glUniformMatrix4fv(location_model_matrix,1,false,glm::value_ptr(model_matrix));
		glBindVertexArray(mesh_vao[nr_elem - 4]);
		glDrawElements(GL_TRIANGLES, mesh_num_indices[nr_elem - 4], GL_UNSIGNED_INT, 0);

		model_matrix = temp;

		// player 1 - mișcare
		if (x_tr < 70.0f && y_tr > -14.0f) {
			x_tr += 0.5f/2;
			y_tr -= 0.1f/2;
		}
		// mișcarea pe semicerc - JUMP
		else if (x_tr < 80.f) {
			earthquake = false;
			if(x_tr == 70.0f) {
				glDeleteBuffers(1,&mesh_vbo[nr_elem-1]);
				glDeleteBuffers(1,&mesh_ibo[nr_elem-1]);
				glDeleteVertexArrays(1,&mesh_vao[nr_elem-1]);
				mesh_vbo.pop_back();
				mesh_ibo.pop_back();
				mesh_vao.pop_back();
				mesh_num_indices.pop_back();
				nr_elem--;
				glDeleteBuffers(1,&mesh_vbo[nr_elem-1]);
				glDeleteBuffers(1,&mesh_ibo[nr_elem-1]);
				glDeleteVertexArrays(1,&mesh_vao[nr_elem-1]);
				mesh_vbo.pop_back();
				mesh_ibo.pop_back();
				mesh_vao.pop_back();
				mesh_num_indices.pop_back();
				nr_elem--;
				
				createMeshManualPlayer(-80.0, 4.0, -2.0, false);
				createMesheFPOS1(-80.0, 6.0, 5.0, false);

			}
			x_tr += 0.5f/2;
			y_tr = sqrt(100 - (x_player + x_tr ) * (x_tr + x_player)) - 14.0f;
		}
		else if (x_tr < 90.f) {
			x_tr += 0.5f/2;
			y_tr = sqrt(100 - (x_player + x_tr ) * (x_tr + x_player)) - 14.0f;
			
		}
		// cădere
		else if (y_player + y_tr > -30.0f) {
			y_tr -= 0.5f;
		}
		// mișcare pe quad
		else if (x_tr < 230) {
			x_tr += 0.5f/2;
		}
		// resetare
		else {
			x_tr = 0.0f;
			y_tr = 0.0f;
			glDeleteBuffers(1,&mesh_vbo[nr_elem-1]);
			glDeleteBuffers(1,&mesh_ibo[nr_elem-1]);
			glDeleteVertexArrays(1,&mesh_vao[nr_elem-1]);
			mesh_vbo.pop_back();
			mesh_ibo.pop_back();
			mesh_vao.pop_back();
			mesh_num_indices.pop_back();
			nr_elem--;
			glDeleteBuffers(1,&mesh_vbo[nr_elem-1]);
			glDeleteBuffers(1,&mesh_ibo[nr_elem-1]);
			glDeleteVertexArrays(1,&mesh_vao[nr_elem-1]);
			mesh_vbo.pop_back();
			mesh_ibo.pop_back();
			mesh_vao.pop_back();
			mesh_num_indices.pop_back();
			nr_elem--;
			
			createMeshManualPlayer(-80.0, 4.0, -2.0, true);
			createMesheFPOS1(-80.0, 6.0, 5.0, false);
		
		}
		// player 2
		// tanslație pe rampă
		if (x_tr2 < 70.0f) {
			x_tr2 += 0.5f/2;
			y_tr2 -= 0.1f/2;
		}
		// JUMP
		else if (x_tr2 < 80.f) {
			if(x_tr2 == 70.0f) {
				glDeleteBuffers(1,&mesh_vbo[nr_elem-3]);
				glDeleteBuffers(1,&mesh_ibo[nr_elem-3]);
				glDeleteVertexArrays(1,&mesh_vao[nr_elem-3]);
				glDeleteBuffers(1,&mesh_vbo[nr_elem-4]);
				glDeleteBuffers(1,&mesh_ibo[nr_elem-4]);
				glDeleteVertexArrays(1,&mesh_vao[nr_elem-4]);
				std::vector<unsigned int> tmp;
				for (int i = 1; i <= 4; i++) {
					if (i <=2) {
						tmp.push_back(mesh_vbo[nr_elem - 1]);
						tmp.push_back(mesh_ibo[nr_elem - 1]);
						tmp.push_back(mesh_vao[nr_elem - 1]);
						tmp.push_back(mesh_num_indices[nr_elem - 1]);
					}
				mesh_vbo.pop_back();
				mesh_ibo.pop_back();
				mesh_vao.pop_back();
				mesh_num_indices.pop_back();
					
				nr_elem--;
				}
				//nr_elem -=2;
				createMeshManualSecondPlayer(-80.0, 14.0, -35.0, false);
				createMesheFPOS2(-80.0, 17.0, -26.0, false);

				mesh_vbo.push_back(tmp[4]);
				mesh_vbo.push_back(tmp[0]);
				mesh_ibo.push_back(tmp[5]);
				mesh_ibo.push_back(tmp[1]);
				mesh_vao.push_back(tmp[6]);
				mesh_vao.push_back(tmp[2]);
				mesh_num_indices.push_back(tmp[7]);
				mesh_num_indices.push_back(tmp[3]);

				nr_elem += 2;
			}
			x_tr2 += 0.5f/2;
			y_tr2 = sqrt(100 - (x_player2 + x_tr2 ) * (x_tr2 + x_player2)) - 24.0f;
		}
		else if (x_tr2 < 90.f) {
			x_tr2 += 0.5f/2;
			y_tr2 = sqrt(100 - (x_player2 + x_tr2 ) * (x_tr2 + x_player2)) - 24.0f;
			
		}
		// cădere
		else if (y_player2 + y_tr2 > -30.0f) {
			y_tr2 -= 0.5f;
		}
		// deplasare pe quad
		else if (x_tr2 < 230) {
			x_tr2 += 0.6f/2;
		}
		//resetare
		else {
			x_tr2 = 0.0f;
			y_tr2 = 0.0f;
			glDeleteBuffers(1,&mesh_vbo[nr_elem-3]);
			glDeleteBuffers(1,&mesh_ibo[nr_elem-3]);
			glDeleteVertexArrays(1,&mesh_vao[nr_elem-3]);
			glDeleteBuffers(1,&mesh_vbo[nr_elem-4]);
			glDeleteBuffers(1,&mesh_ibo[nr_elem-4]);
			glDeleteVertexArrays(1,&mesh_vao[nr_elem-4]);
			
			std::vector<unsigned int> tmp;
			for (int i = 1; i <= 4; i++) {
					if (i <=2) {
					tmp.push_back(mesh_vbo[nr_elem - 1]);
					tmp.push_back(mesh_ibo[nr_elem - 1]);
					tmp.push_back(mesh_vao[nr_elem - 1]);
					tmp.push_back(mesh_num_indices[nr_elem - 1]);
					}
				mesh_vbo.pop_back();
				mesh_ibo.pop_back();
				mesh_vao.pop_back();
				mesh_num_indices.pop_back();
					
				nr_elem--;
			}
			createMeshManualSecondPlayer(-80.0, 14.0, -35.0, true);
			createMesheFPOS2(-80.0, 17.0, -26.0, false);

			mesh_vbo.push_back(tmp[4]);
			mesh_vbo.push_back(tmp[0]);
			mesh_ibo.push_back(tmp[5]);
			mesh_ibo.push_back(tmp[1]);
			mesh_vao.push_back(tmp[6]);
			mesh_vao.push_back(tmp[2]);
			mesh_num_indices.push_back(tmp[7]);
			mesh_num_indices.push_back(tmp[3]);
			nr_elem +=2;
		}
		
		
	
	}
	//functie chemata dupa ce am terminat cadrul de desenare (poate fi folosita pt modelare/simulare)
	void notifyEndFrame(){}
	//functei care e chemata cand se schimba dimensiunea ferestrei initiale
	void notifyReshape(int width, int height, int previos_width, int previous_height){
		//reshape
		if(height==0) height=1;
		glViewport(0,0,width,height);
		projection_matrix = glm::perspective(90.0f, (float)width/(float)height,0.1f, 10000.0f);
	}


	//--------------------------------------------------------------------------------------------
	//functii de input output --------------------------------------------------------------------
	
	//tasta apasata
	void notifyKeyPressed(unsigned char key_pressed, int mouse_x, int mouse_y){
		if(key_pressed == 27) lab::glut::close();	//ESC inchide glut si 
		if(key_pressed == 32) {
			//SPACE reincarca shaderul si recalculeaza locatiile (offseti/pointeri)
			glDeleteProgram(gl_program_shader);
			gl_program_shader = lab::loadShader("shadere\\shader_vertex.glsl", "shadere\\shader_fragment.glsl");
			location_model_matrix = glGetUniformLocation(gl_program_shader, "model_matrix");
			location_view_matrix = glGetUniformLocation(gl_program_shader, "view_matrix");
			location_projection_matrix = glGetUniformLocation(gl_program_shader, "projection_matrix");
		}
		if(key_pressed == 'w'){
			glPolygonMode(GL_FRONT_AND_BACK, (GL_LINE));
		}
		if(key_pressed == 's'){
			glPolygonMode(GL_FRONT_AND_BACK, (GL_FILL));
		}
		if(key_pressed == 'e'){
			earthquake = true;
		}
		if(key_pressed == 'r'){
			earthquake = false;
		}
		// pt rotație
		/*
		if(key_pressed == GLUT_KEY_UP) {
			std::cout << "UP\n";
			if (x_tr < 70.0f && y_tr > -14.0f) {
				x_tr += 0.5f;
				y_tr -= 0.1f;
				//std::cout << "x_tr = "<< x_tr << "y_tr = " << y_tr <<"\n";
			}
			else {
				x_tr = 0.0f;
				y_tr = 0.0f;
			}
		}
		if(key_pressed == GLUT_KEY_LEFT) {
			angle -= 0.1f;
			std::cout << "LEFT\n";
		}
		if(key_pressed == GLUT_KEY_RIGHT) {
			angle += 0.1f;
			std::cout << "RIGHT\n";
		}
		*/
		if(key_pressed == 'c') {
			std::cout<<"cameras-c\n";
			cameras ++;
			
		}
		// translații + rotații pt camera FP de sus
		if(key_pressed == '1') { if(cameras % 7 == 0) cam.translateForward(1.0f);}
		if(key_pressed == '3') { if(cameras % 7 == 0) cam.translateRight(-1.0f); }
		if(key_pressed == '2') { if(cameras % 7 == 0) cam.translateForward(-1.0f); }
		if(key_pressed == '4') { if(cameras % 7 == 0) cam.translateRight(1.0f); }
		if(key_pressed == '5') { if(cameras % 7 == 0) cam.translateUpword(1.0f); }
		if(key_pressed == '6') { if(cameras % 7 == 0) cam.translateUpword(-1.0f); }
		if(key_pressed == '7') { if(cameras % 7 == 0) cam.rotateFPSoY(0.056f); }
		if(key_pressed == '8') { if(cameras % 7 == 0) cam.rotateFPSoY(-0.05f); }
		if(key_pressed == '9') { if(cameras % 7 == 0) cam.rotateFPSoZ(-0.056f); }
		if(key_pressed == '0') { if(cameras % 7 == 0) cam.rotateFPSoZ(0.05f); }
		if(key_pressed == '-') { if(cameras % 7 == 0) cam.rotateFPSoX(0.056f); }
		if(key_pressed == '=') { if(cameras % 7 == 0) cam.rotateFPSoX(-0.05f); }
		
	}
	//tasta ridicata
	void notifyKeyReleased(unsigned char key_released, int mouse_x, int mouse_y){	}
	//tasta speciala (up/down/F1/F2..) apasata
	void notifySpecialKeyPressed(int key_pressed, int mouse_x, int mouse_y){
		if(key_pressed == GLUT_KEY_F1) lab::glut::enterFullscreen();
		if(key_pressed == GLUT_KEY_F2) lab::glut::exitFullscreen();
	}
	//tasta speciala ridicata
	void notifySpecialKeyReleased(int key_released, int mouse_x, int mouse_y){}
	//drag cu mouse-ul
	void notifyMouseDrag(int mouse_x, int mouse_y){ }
	//am miscat mouseul (fara sa apas vreun buton)
	void notifyMouseMove(int mouse_x, int mouse_y){ }
	//am apasat pe un boton
	void notifyMouseClick(int button, int state, int mouse_x, int mouse_y){ 
		std::cout<<"Mouse x = "<<mouse_x<<std::endl;
		std::cout<<"Mouse y = "<<mouse_y<<std::endl;
	}
	//scroll cu mouse-ul
	void notifyMouseScroll(int wheel, int direction, int mouse_x, int mouse_y){ std::cout<<"Mouse scroll"<<std::endl;}
};

int main(){
	//initializeaza GLUT (fereastra + input + context OpenGL)
	lab::glut::WindowInfo window(std::string("Ski Jumping"),800,600,100,100,true);
	//lab::glut::ContextInfo context(3,3,false);
	lab::glut::ContextInfo context(2,1,false);
	lab::glut::FramebufferInfo framebuffer(true,true,true,true);
	lab::glut::init(window,context, framebuffer);

	//initializeaza GLEW (ne incarca functiile openGL, altfel ar trebui sa facem asta manual!)
	glewExperimental = true;
	glewInit();
	std::cout<<"GLEW:initializare"<<std::endl;

	//creem clasa noastra si o punem sa asculte evenimentele de la GLUT
	//DUPA GLEW!!! ca sa avem functiile de OpenGL incarcate inainte sa ii fie apelat constructorul (care creeaza obiecte OpenGL)
	Scene game;//, mylab2;
	
	lab::glut::setListener(&game);
	//run
	lab::glut::run();
	return 0;
}