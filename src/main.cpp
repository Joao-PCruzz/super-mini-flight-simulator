// Bibliotecas essenciais
#include <GL/gl.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <array>
#include <cmath>

// Comando para compilar o código:
// make run ARGS="<raio_terra> <x_sol> <y_sol> <z_sol>"
// Se utilizar somente o make run os parâmetros serão o default (1.0, 0.0, 0.0 5.0) 

// Struct que cria um vetor com 3 coordenadas e a flag de ponto ou vetor com operações básicas
struct Vetor3 {
    std::array<float, 4> coordinates;
    // Soma dois vetores
    Vetor3 operator+(const Vetor3& v) const { return {coordinates[0] + v.coordinates[0], coordinates[1] + v.coordinates[1], coordinates[2] + v.coordinates[2]}; }
    // Subtração de dois vetores
    Vetor3 operator-(const Vetor3& v) const { return {coordinates[0] - v.coordinates[0], coordinates[1] - v.coordinates[1], coordinates[2] - v.coordinates[2]}; }
    // Multiplica um vetor por um escalar
    Vetor3 operator*(float escalar) const { return {coordinates[0] * escalar, coordinates[1] * escalar, coordinates[2] * escalar}; } 
    // Calcula o tamanho real
    float tamanho() const { return std::sqrt(coordinates[0]*coordinates[0] + coordinates[1]*coordinates[1] + coordinates[2]*coordinates[2]); }
    // Aplica a normalização ao vetor
    Vetor3 normalizado() const {
        float tam = tamanho();
        if (tam == 0.0f) return {0.0f, 0.0f, 0.0f, 0.0f};
        return {coordinates[0]/tam, coordinates[1]/tam, coordinates[2]/tam, 0.0f};
    }
};

 // Operação de dot (produto escalar) entre vetores
float dot(const Vetor3& v, const Vetor3& w) { return (w.coordinates[0] * v.coordinates[0]) + (w.coordinates[1] * v.coordinates[1]) + (w.coordinates[2] * v.coordinates[2]); }

Vetor3 cross(const Vetor3& v, const Vetor3& w)
{
    Vetor3 u;
    u.coordinates[0] = v.coordinates[1] * w.coordinates[2] - w.coordinates[1] * v.coordinates[2];
    u.coordinates[1] = v.coordinates[2] * w.coordinates[0] - w.coordinates[2] * v.coordinates[0];
    u.coordinates[2] = v.coordinates[0] * w.coordinates[1] - w.coordinates[0] * v.coordinates[1];
    u.coordinates[3] = 0.0f;
    return u;
}

// Enum para os estados do avião
enum Estado_aviao {
    no_chao,
    subindo,
    no_ar,
    descendo
};

// Struct para representar o aviao, apoiando suas operações
struct Aviao {
    Vetor3 posicao;
    Vetor3 frente;
    Vetor3 cima;
    float velocidade = 0.0f;
    float altura_atual = 0.0f;
    Estado_aviao estado = no_chao;
    Vetor3 direita()
    {
        return cross(frente, cima);
    }
};
Aviao aviao;

// Variável global para capturar o modo de exibição das formas
bool modo_visual = false; 
// Variável para evitar o corte de planos
float plano_proximo = 0.1f; 

// Função para desenhar a esfera, seguindo uma lógica em camadas e utilizando laços aninhados
void desenharEsfera(float raio, int latitude, int longitude) {
    // Laço para criar dois aneis paralelos, calculando sua latitude de -90° até 90°
    for(int i = 0; i < latitude; i++) {
        float lat0 = M_PI * (-0.5f + (float)i / latitude);
        float z0 = raio * sinf(lat0); // Calcular a altura vertical do eixo Z no anel
        float zr0 = raio * cosf(lat0); // Calcular o raio do anel desenhado

        float lat1 = M_PI * (-0.5 + (float)(i + 1) / latitude);
        float z1 = raio * sinf(lat1);
        float zr1 = raio * cosf(lat1);

        // Laço para calcular a longitude em um ângulo de 0° até 360° e desenhar a malha
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

void atualizarFisica(Aviao& aviao, float dt, float raio_planeta) {
    // Grandezas proporcionais ao tamanho planeta
    float altura_subida = raio_planeta / 2.0f;
    float taxa_vertical = raio_planeta * 0.1f;
    float velocidade_subida = raio_planeta * 0.1f;
    float velocidade_maxima = (float)M_PI * (raio_planeta + altura_subida) / 10.0f;
    float velocidade_minima = 0.1f * velocidade_maxima;
    float desaceleracao = velocidade_maxima / (altura_subida / taxa_vertical);

    if (aviao.estado == subindo) {
        aviao.velocidade = velocidade_subida;
        // Realiza a subida do avião em relação a taxa
        aviao.altura_atual += taxa_vertical * dt;

        if(aviao.altura_atual >= altura_subida) {
            aviao.altura_atual = altura_subida;
            aviao.estado = no_ar;
        }
    }

    else if (aviao.estado == no_ar) {
        // Condicionais para evitar bugs no avião
        aviao.velocidade = std::min(aviao.velocidade, velocidade_maxima);
        aviao.velocidade = std::max(velocidade_minima, aviao.velocidade);
    } 

    else if (aviao.estado == descendo) {
        // Desacelera o avião para ele descer e reduz sua altura
        aviao.velocidade -= desaceleracao * dt;
        aviao.altura_atual -= taxa_vertical * dt;

        // Condicionais para evitar bugs no avião
        if(aviao.velocidade <= 0.0f) aviao.velocidade = 0.0f;
        if(aviao.altura_atual <= 0.0f) {
            aviao.altura_atual = 0.0f;
            aviao.estado = no_chao;
            aviao.velocidade = 0.0f;
        }
    } 

    else if (aviao.estado == no_chao) {
        aviao.velocidade = 0.0f;
    }
   
    // Altera a posição do avião ao longo do tempo
    aviao.posicao = aviao.posicao + (aviao.frente * aviao.velocidade * dt);

    // Renormalizando as coordenadas
    float altitude_desejada = raio_planeta + aviao.altura_atual;
    aviao.posicao = aviao.posicao.normalizado() * altitude_desejada;
    aviao.cima = aviao.posicao.normalizado();
    aviao.frente = ( (aviao.frente - aviao.cima * dot(aviao.frente, aviao.cima)).normalizado() );
}

void atualizarMovimento(Aviao& aviao, float dt, GLFWwindow *window)
{
    if (aviao.estado != no_ar)
    {
        return;
    }
    float aceleracao = aviao.velocidade * 0.2f;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT) != GLFW_PRESS && aviao.velocidade != 0.0f)
    {
        Vetor3 direcao = aviao.direita();
        aviao.frente = (aviao.frente * aviao.velocidade + direcao * aceleracao * dt).normalizado();
    }
    else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_RIGHT) != GLFW_PRESS && aviao.velocidade != 0.0f)
    {
        Vetor3 direcao = aviao.direita() * -1.0f;
        aviao.frente = (aviao.frente * aviao.velocidade + direcao * aceleracao * dt).normalizado();
    }

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_DOWN) != GLFW_PRESS)
    {
        aviao.velocidade += aceleracao * dt;
    }
    else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_UP) != GLFW_PRESS)
    {
        aviao.velocidade -= aceleracao * dt;
    }
}

void desenhar(float raio_planeta, float raio_sol, float sol_x, float sol_y, float sol_z, Aviao& aviao) {
    // Limpando os buffers essenciais para figuras 3D
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Alterando para o ModelView para poder realizar operações de transformação
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Define onde a câmera (avião) está, para onde ela olha e onde é o seu "cima"
    Vetor3 alvo = aviao.posicao + aviao.frente - aviao.cima * 0.5f;
    gluLookAt(aviao.posicao.coordinates[0], aviao.posicao.coordinates[1], aviao.posicao.coordinates[2],
            alvo.coordinates[0], alvo.coordinates[1], alvo.coordinates[2],
            aviao.cima.coordinates[0], aviao.cima.coordinates[1], aviao.cima.coordinates[2]);

    // Desenhar o planeta
    glPushMatrix();
        glColor3ub(0, 255, 0);
        desenharEsfera(raio_planeta, 20, 40);
    glPopMatrix();

    // Desenhar sol
    glPushMatrix();
        glTranslatef(sol_x, sol_y, sol_z);
        glColor3ub(255, 255, 0);
        desenharEsfera(raio_sol, 20, 40);
    glPopMatrix();
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

    float limite = 100000.0f;
    float *limite_ptr = reinterpret_cast<float *>(glfwGetWindowUserPointer(window));
    if (limite_ptr)
    {
        limite = *limite_ptr;
    }

    // Alteração da perspectiva de acordo com alguns detalhes dos sistema
    gluPerspective(60, aspect, plano_proximo, limite);
    glMatrixMode(GL_MODELVIEW);
}

// Função para capturar a tecla R e mudar a exibição das formas
void processarTeclado(GLFWwindow* window, int tecla, int scancode, int acao, int mods) {
    // Alterar o modo de visualização das eseferas entre sólido/malha
    if(tecla == GLFW_KEY_R && acao == GLFW_PRESS) {
        modo_visual = !modo_visual;
        if(modo_visual) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Modo malha
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Modo solido
        }
    }

    // Decolar o avião a partir da tecla 'D'
    if(tecla == GLFW_KEY_D && acao == GLFW_PRESS) {
        if(aviao.estado == no_chao) {
            aviao.estado = subindo;
        }
    }

    // Aterrisar o avião a partir da tecla 'A'
    if(tecla == GLFW_KEY_A && acao == GLFW_PRESS) {
        if(aviao.estado == no_ar) {
            aviao.estado = descendo;
        }
    }
}


void iniciar(float limite_visao) {
    // Cor de limpeza: preto
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glEnable(GL_DEPTH_TEST); // Ativa o teste de profundidade dos Z-Buffers

    // Configura a projeção
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(60.0, 1.0, plano_proximo, limite_visao);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char *argv[]) {
    // Variáveis da posição do sol e raio do planeta
    float raio_planeta = 1.0f, sol_x = 0.0f, sol_y = 0.0f, sol_z = 5.0f;
    if (argc >= 5)
    {
        raio_planeta = std::stof(argv[1]);
        sol_x = std::stof(argv[2]), sol_y = std::stof(argv[3]), sol_z = std::stof(argv[4]);
    }
    if (raio_planeta <= 0.0f)
    {
        std::cout << "O raio do planeta deve ser positivo\n";
        return 0;
    }
    if (sol_x * sol_x + sol_y * sol_y + sol_z * sol_z <= raio_planeta * raio_planeta)
    {
        std::cout << "Sol nao pode estar dentro do planeta\n";
        return 0; 
    }

    // Plano proximo proporcional ao raio
    plano_proximo = 0.1f * raio_planeta;
    // Raio do sol
    float raio_sol = 0.5f * raio_planeta;
    // Distancia do sol em relação a origem
    float distancia_sol = sqrtf(pow(sol_x, 2.0f) + pow(sol_y, 2.0f) + pow(sol_z, 2.0f));
    // Distancia limite da visão da camera
    float limite_visao = raio_planeta * 1.5f + distancia_sol + raio_sol * 2.0f;
    float* limite_visao_ptr = &limite_visao;

    //Incializando o GLFW
    if(!glfwInit()) return -1;

    // Criação da janela
    GLFWwindow* window = glfwCreateWindow(600, 600, "Super Mini Fligth Simulator - Fase 2", NULL, NULL);
    // Libera recursos se algo der errado na janela
    if(!window) {
        glfwTerminate();
        return -1;
    }
     // Associa janela criada ao contexto opengl a ser usado
    glfwMakeContextCurrent(window);
    // Registra o callback para detectar mudanças de tamanho da janela
    glfwSetWindowUserPointer(window, reinterpret_cast<float *>(limite_visao_ptr));
    glfwSetFramebufferSizeCallback(window, redimensionar);
    // Função global para processar os sinais do teclado, pois o GLFW permite apenas uma função de Callback
    glfwSetKeyCallback(window, processarTeclado);  
    
    iniciar(limite_visao);

    aviao.posicao = {0.0f, raio_planeta, 0.0f, 1.0};
    aviao.cima = aviao.posicao.normalizado();
    aviao.frente = {0.0f, 0.0f, -1.0f, 0.0f};
    float tempo_anterior = (float)glfwGetTime();
    // loop de execução
    while (!glfwWindowShouldClose(window)) {

        // Variáveis para auxiliar o movimento do avião
        float tempo_atual = (float)glfwGetTime();
        float dt = tempo_atual - tempo_anterior;
        tempo_anterior = tempo_atual;

        // Evita um salto do avião se a janela travar
        if(dt > 0.1f) dt = 0.1f;

        atualizarMovimento(aviao, dt, window);

        // Atualizar física do avião para o seu movimento
        atualizarFisica(aviao, dt, raio_planeta);

		// Desenha as esferas na tela
        desenhar(raio_planeta, raio_sol, sol_x, sol_y, sol_z, aviao);

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