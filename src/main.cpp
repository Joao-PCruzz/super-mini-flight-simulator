// Bibliotecas padrão
#include <GL/gl.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>

// Função Principal - Criação da Janela e contexto
int main() {
    //Incializando o GLFW
    if(!glfwInit()) {
        return -1;
    }

    // Criação da janela
    GLFWwindow* window = glfwCreateWindow(600, 600, "Teste - Fase 1", NULL, NULL);
    // Libera recursos se algo der errado na janela
    if(!window) {
        glfwTerminate();
        return -1;
    }
     // Associa janela criada ao contexto opengl a ser usado
    glfwMakeContextCurrent(window);

    // loop de execução
    while (!glfwWindowShouldClose(window)) {
		// Destinado as demais funções do programa...
        
        // Troca de buffers
        glfwSwapBuffers(window);
        // Processa eventos do loop
        glfwPollEvents();
    }
    // Termina todas as conexões e contextos criados pelo opengl
    glfwTerminate();
    
    return 0;
}