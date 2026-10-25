// Bibliotecas essenciais
#include <GL/gl.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>

//Comando para compilar o código:
// g++ src/main.cpp -o bin/simulador -lglfw -lGL -lGLU
// ./bin/simulador

// Variáveis globais da posição do sol e raio do planeta
float raio_planeta = 1.0f;
float sol_x = 3.0f, sol_y = 2.0f, sol_z = -2.0f;

// Variável global para capturar o modo de exibição das formas
bool modo_visual = false;

// Função para desenhar a esfera, seguindo uma lógica em camadas e utilizando laços aninhados
void desenharEsfera(float raio, int latitude, int longitude) {
    // Função para criar dois aneis paralelos, calculando sua latitude de -90° até 90°
    for(int i = 0; i < latitude; i++) {
        float lat0 = M_PI * (-0.5f + (float)i / latitude);
        float z0 = raio * sinf(lat0); // Calcular a altura vertical do eixo Z no anel
        float zr0 = raio * cosf(lat0); // Calcular o raio do anel desenhado

        float lat1 = M_PI * (-0.5 + (float)(i + 1) / latitude);
        float z1 = raio * sinf(lat1);
        float zr1 = raio * cosf(lat1);

        // Função para calcular a longitude em um ângulo de 0° até 360° e desenhar a malha
        glBegin(GL_TRIANGLE_STRIP);
            for (int j = 0; j <= longitude; j++) {
                float lng = 2.0f * M_PI * (float)j / longitude;
                float x = cosf(lng); // Componente de direção x
                float y = sinf(lng); // Componente de direção y

                // Criação dos vértices
                glVertex3f(x * zr0, y * zr0, z0);
                glVertex3f(x * zr1, y * zr1, z1);
            }
        glEnd();
    }
}

void desenhar() {
    // Limpando os buffers essenciais para figuras 3D
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Alterando para o ModelView para poder realizar operações de transformação
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Posiciona a câmera fastado em Z
    // A distCam é responsável por definir a distância da câmera me relação ao planeta
    float distCam = raio_planeta * 4.0f + 5.0f;
    gluLookAt(0.0, 0.0, distCam,
            0.0, 0.0, 0.0,
            0.0, 1.0, 0.0);

    // Rotacionar o mundo inteiro para ver a câmera em ação (para ver o mundo como um todo)
    float tempo = (float)glfwGetTime();
    glRotatef(tempo * 25.0f, 0.0f, 1.0f, 0.0f); // Gira em torno do eio y

    // Desenhar o planeta
    glPushMatrix();
        glColor3ub(0, 255, 0);
        desenharEsfera(raio_planeta, 20, 20);
    glPopMatrix();

    // Desenhar sol
    glPushMatrix();
        glTranslatef(sol_x, sol_y, sol_z);
        glColor3ub(255, 255, 0);
        desenharEsfera(0.5f, 10, 10);
    glPopMatrix();

    // Troca os buffers após o desenho
    glfwSwapBuffers(glfwGetCurrentContext());
}

// Função para redimensionar a tela em caso de alteração de tamanho
void redimensionar(GLFWwindow* window, int largura, int altura) {
    // Evita divisão por zero caso a janela seja minimizada
    if (altura == 0) altura = 1;
    // Atualiza a área de renderização do OpenGL para os novos pixels
    glViewport(0, 0, largura, altura);

    // Recalcula a projeção mantendo a proporção correta
    float aspect = (float)largura / (float)altura;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Alteração da perspectiva de acordo com o aspecto
    gluPerspective(60, aspect, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
}

// Função para capturar a tecla R e mudar a exibição das formas
void key_R(GLFWwindow* window, int tecla, int scancode, int acao, int mods) {
    if(tecla == GLFW_KEY_R && acao == GLFW_PRESS) {
        modo_visual = !modo_visual;
        if(modo_visual) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Modo malha
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Modo solido
        }
    }
}

void iniciar() {
    // Cor de limpeza: preto
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glEnable(GL_DEPTH_TEST); // Ativa o teste de profundidade dos Z-Buffers

    // Configura a projeção
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(60.0, 1.0, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

int main() {
    //Incializando o GLFW
    if(!glfwInit()) return -1;

    // Criação da janela
    GLFWwindow* window = glfwCreateWindow(600, 600, "Super Mini Fligth Simulator - Fase 1", NULL, NULL);
    // Libera recursos se algo der errado na janela
    if(!window) {
        glfwTerminate();
        return -1;
    }
     // Associa janela criada ao contexto opengl a ser usado
    glfwMakeContextCurrent(window);
    // Registra o callback para detectar mudanças de tamanho da janela
    glfwSetFramebufferSizeCallback(window, redimensionar);
    // Registra o callback da tecla R 
    glfwSetKeyCallback(window, key_R);
    
    iniciar();
    // loop de execução
    while (!glfwWindowShouldClose(window)) {
		// Destinado a funções do programa
        desenhar();
        // Troca de buffers
        glfwSwapBuffers(window);
        // Processa eventos do loop
        glfwPollEvents();
    }
    // Termina todas as conexões e contextos criados pelo opengl
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}