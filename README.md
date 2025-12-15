# 🚀 Sistema Multitarefa com FreeRTOS na ESP32
## Monitoramento MPU6050 com Processamento Multinúcleo

[![ESP32](https://img.shields.io/badge/ESP32-FreeRTOS-blue.svg)](https://www.freertos.org/)
[![Wokwi](https://img.shields.io/badge/Simulador-Wokwi-green.svg)](https://wokwi.com/)
[![License](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

---

## 📋 Índice

- [Visão Geral](#-visão-geral)
- [Objetivos do Projeto](#-objetivos-do-projeto)
- [Arquitetura do Sistema](#-arquitetura-do-sistema)
- [Componentes Utilizados](#-componentes-utilizados)
- [Conceitos FreeRTOS](#-conceitos-freertos)
- [Descrição das Tasks](#-descrição-das-tasks)
- [Sincronização e Proteção de Dados](#-sincronização-e-proteção-de-dados)
- [Código Fonte Comentado](#-código-fonte-comentado)
- [Montagem e Testes](#-montagem-e-testes)
- [Resultados](#-resultados)
- [Conclusões](#-conclusões)

---

## 🎯 Visão Geral

Este projeto implementa um **sistema multitarefa em tempo real** utilizando **FreeRTOS** na plataforma **ESP32**, explorando os recursos de **processamento multinúcleo** e **sincronização entre tarefas**. A aplicação desenvolvida consiste em um sistema de monitoramento de dados inerciais através do sensor **MPU6050** (acelerômetro e giroscópio), com exibição simultânea das informações em um display LCD I2C 20x4.

### 🎨 Características Principais

- ✅ **Processamento Multinúcleo**: Tarefas distribuídas entre Core 0 e Core 1
- ✅ **Prioridades Diferenciadas**: Sistema de prioridades configurável
- ✅ **Sincronização com Mutex**: Proteção de recursos compartilhados
- ✅ **Leitura de Sensores I2C**: Comunicação com MPU6050
- ✅ **Interface Visual**: Display LCD 20x4 para visualização em tempo real

---

## 🎯 Objetivos do Projeto

### Objetivos Gerais
- Demonstrar a capacidade do FreeRTOS em gerenciar tarefas concorrentes
- Explorar o processamento multinúcleo da ESP32
- Implementar mecanismos de sincronização entre tarefas

### Objetivos Específicos
1. Criar **Task de Alta Prioridade** para aquisição de dados do MPU6050 (Core 1)
2. Criar **Task de Baixa Prioridade** para exibição no LCD (Core 0)
3. Implementar **Mutex** para proteger estrutura de dados compartilhada
4. Garantir atualização contínua e consistente dos dados exibidos

---

## 🏗️ Arquitetura do Sistema

### Diagrama de Blocos

```
┌─────────────────────────────────────────────────────────────┐
│                         ESP32 (Dual Core)                    │
├─────────────────────────────────┬───────────────────────────┤
│         CORE 1                  │         CORE 0            │
│    (Alta Prioridade)            │    (Baixa Prioridade)     │
│                                 │                           │
│  ┌──────────────────────┐       │  ┌──────────────────────┐│
│  │   Task 1: MPU6050    │       │  │   Task 2: LCD        ││
│  │   Reader             │       │  │   Display            ││
│  │                      │       │  │                      ││
│  │ • Lê sensor MPU6050  │       │  │ • Formata dados      ││
│  │ • Temperatura        │       │  │ • Atualiza LCD       ││
│  │ • Acelerômetro (XYZ) │       │  │ • Refresh 1s         ││
│  │ • Giroscópio (XYZ)   │       │  │                      ││
│  │ • Refresh 500ms      │       │  │                      ││
│  └──────────┬───────────┘       │  └──────────┬───────────┘│
│             │                   │             │            │
│             └───────────────────┴─────────────┘            │
│                          │                                 │
│                   ┌──────▼──────┐                         │
│                   │    MUTEX    │                         │
│                   │  (Semáforo) │                         │
│                   └──────┬──────┘                         │
│                          │                                 │
│                   ┌──────▼──────────┐                     │
│                   │ Estrutura MPU6050│                     │
│                   │ (Dados Compart.) │                     │
│                   └──────────────────┘                     │
└─────────────────────────────────────────────────────────────┘
                          │
          ┌───────────────┴───────────────┐
          │                               │
     ┌────▼─────┐                  ┌──────▼──────┐
     │ MPU6050  │                  │  LCD 20x4   │
     │  Sensor  │                  │   Display   │
     └──────────┘                  └─────────────┘
          I2C                           I2C
```

### Distribuição de Tarefas por Núcleo

| Task | Núcleo | Prioridade | Período | Descrição |
|------|--------|------------|---------|-----------|
| **mpu6050Task** | Core 1 | Alta (configMAX_PRIORITIES - 2) | 500ms | Leitura do sensor MPU6050 |
| **lcdTask** | Core 0 | Baixa (tskIDLE_PRIORITY + 1) | 1000ms | Atualização do display LCD |

---

## 🔧 Componentes Utilizados

### Hardware
- **ESP32 DevKit V1** - Microcontrolador dual-core
- **MPU6050** - Sensor IMU (6 DOF: 3 eixos acelerômetro + 3 eixos giroscópio)
- **Display LCD 20x4 com I2C** - Interface de visualização
- **Protoboard e jumpers** - Montagem do circuito

### Bibliotecas
```cpp
#include <Adafruit_MPU6050.h>    // Biblioteca para MPU6050
#include <Adafruit_Sensor.h>     // Abstração de sensores
#include <Wire.h>                // Comunicação I2C
#include <LiquidCrystal_I2C.h>   // Controle do LCD I2C
```

### Conexões I2C

| Componente | Endereço I2C | SDA | SCL |
|------------|--------------|-----|-----|
| MPU6050 | 0x68 | GPIO 21 | GPIO 22 |
| LCD 20x4 | 0x27 | GPIO 21 | GPIO 22 |

---

## 📚 Conceitos FreeRTOS

### O que é FreeRTOS?

**FreeRTOS** (Free Real-Time Operating System) é um kernel de sistema operacional de tempo real projetado para sistemas embarcados. Ele fornece:

- **Escalonamento de tarefas** (scheduler)
- **Gerenciamento de memória**
- **Sincronização entre tarefas** (semáforos, mutexes, filas)
- **Comunicação entre tarefas**

### 🔄 Tasks vs Processos vs Threads

#### Comparação Fundamental

| Aspecto | **Processos (Linux)** | **Threads (POSIX)** | **Tasks (FreeRTOS)** |
|---------|----------------------|---------------------|----------------------|
| **Espaço de Memória** | Isolado (cada processo tem seu próprio espaço) | Compartilhado dentro do processo | Compartilhado (toda a RAM é acessível) |
| **Overhead** | Alto (troca de contexto pesada) | Médio | Baixo (ideal para microcontroladores) |
| **Comunicação** | IPC (pipes, sockets, shared memory) | Variáveis globais, mutexes | Filas, semáforos, mutexes |
| **Criação** | `fork()`, `exec()` | `pthread_create()` | `xTaskCreate()` |
| **Contexto** | Sistema operacional completo | SO com suporte a threads | Kernel minimalista |
| **Uso de Recursos** | Pesado (MB de RAM) | Moderado (KB de RAM) | Leve (centenas de bytes) |

#### Vantagens do FreeRTOS em Microcontroladores

1. **Menor Footprint**: FreeRTOS ocupa poucos KB de memória
2. **Previsibilidade**: Comportamento determinístico em tempo real
3. **Simplicidade**: API enxuta e direta
4. **Eficiência**: Troca de contexto extremamente rápida
5. **Sem MMU**: Não necessita de Memory Management Unit

### 🔒 Mutex (Exclusão Mútua)

Um **Mutex** é um mecanismo de sincronização que garante acesso exclusivo a um recurso compartilhado.

#### Como Funciona?

```
Task 1                  Mutex                   Task 2
  |                       |                       |
  |--xSemaphoreTake()---->|                       |
  |<------BLOQUEADO-------|                       |
  |                       |                       |
  | [Acessa Recurso]      |                       |
  |                       |                       |
  |--xSemaphoreGive()---->|                       |
  |                       |<--xSemaphoreTake()---|
  |                       |-------BLOQUEADO----->|
  |                       |                       |
  |                       |      [Acessa Recurso] |
  |                       |                       |
  |                       |<--xSemaphoreGive()---|
```

#### No Nosso Projeto

```cpp
SemaphoreHandle_t xMutexMPU6050;  // Declara o mutex
xMutexMPU6050 = xSemaphoreCreateMutex();  // Cria o mutex

// Task 1 (Escritor)
if (xSemaphoreTake(xMutexMPU6050, timeOut) == pdPASS) {
    // Escreve dados do sensor
    mpu6050.temp = temp.temperature;
    // ...
    xSemaphoreGive(xMutexMPU6050);  // Libera o mutex
}

// Task 2 (Leitor)
if (xSemaphoreTake(xMutexMPU6050, timeOut) == pdPASS) {
    // Lê dados protegidos
    snprintf(line1, lcdChar + 1, "Temp: %.1f C", mpu6050.temp);
    // ...
    xSemaphoreGive(xMutexMPU6050);  // Libera o mutex
}
```

### ⚙️ Processamento Multinúcleo na ESP32

A ESP32 possui **2 núcleos** (Xtensa LX6):
- **Core 0**: Por padrão, executa tarefas do sistema (WiFi, Bluetooth)
- **Core 1**: Por padrão, executa o código do usuário (loop)

#### Alocação de Tasks em Núcleos Específicos

```cpp
xTaskCreatePinnedToCore(
    mpu6050Task,              // Função da task
    "MPU6050_Reader",         // Nome da task
    4096,                     // Tamanho da stack (bytes)
    NULL,                     // Parâmetro passado
    configMAX_PRIORITIES - 2, // Prioridade ALTA
    NULL,                     // Handle da task
    1                         // CORE 1
);

xTaskCreatePinnedToCore(
    lcdTask,                  // Função da task
    "LCD_Display",            // Nome da task
    4096,                     // Tamanho da stack
    NULL,                     // Parâmetro
    tskIDLE_PRIORITY + 1,     // Prioridade BAIXA
    NULL,                     // Handle
    0                         // CORE 0
);
```

---

## 🔬 Descrição das Tasks

### Task 1: `mpu6050Task` (Leitura do Sensor)

**Núcleo**: Core 1  
**Prioridade**: Alta (`configMAX_PRIORITIES - 2`)  
**Período**: 500ms

#### Responsabilidades
1. Inicializar o sensor MPU6050
2. Ler temperatura do sensor
3. Ler aceleração nos 3 eixos (X, Y, Z)
4. Ler velocidade angular nos 3 eixos (X, Y, Z)
5. Armazenar dados na estrutura compartilhada (protegida por mutex)

#### Fluxo de Execução

```
    ┌─────────────────────┐
    │   Inicialização     │
    │   MPU6050           │
    └──────────┬──────────┘
               │
               ▼
    ┌─────────────────────┐
    │   Loop Infinito     │
    └──────────┬──────────┘
               │
               ▼
    ┌─────────────────────┐
    │ Aguarda Mutex       │
    │ (Timeout 1s)        │
    └──────────┬──────────┘
               │
               ▼
    ┌─────────────────────┐
    │ Lê Sensor MPU6050   │
    │ • Temperatura       │
    │ • Acelerômetro      │
    │ • Giroscópio        │
    └──────────┬──────────┘
               │
               ▼
    ┌─────────────────────┐
    │ Atualiza Estrutura  │
    │ mpu6050 (global)    │
    └──────────┬──────────┘
               │
               ▼
    ┌─────────────────────┐
    │ Libera Mutex        │
    └──────────┬──────────┘
               │
               ▼
    ┌─────────────────────┐
    │ Delay 500ms         │
    └──────────┬──────────┘
               │
               └────────────► (Retorna ao loop)
```

### Task 2: `lcdTask` (Exibição no Display)

**Núcleo**: Core 0  
**Prioridade**: Baixa (`tskIDLE_PRIORITY + 1`)  
**Período**: 1000ms

#### Responsabilidades
1. Inicializar o display LCD I2C
2. Formatar os dados do sensor em strings
3. Atualizar as 4 linhas do display LCD 20x4
4. Exibir tempo de execução (uptime)

#### Layout do Display

```
┌────────────────────┐
│MPU6050 T:123 s    │ ← Linha 0: Título + Tempo
│Temp: 25.3 C       │ ← Linha 1: Temperatura
│ACC: X1.2 Y0.5 Z9.8│ ← Linha 2: Acelerômetro
│GYRO: X0.1 Y-0.2 Z0│ ← Linha 3: Giroscópio
└────────────────────┘
```

#### Fluxo de Execução

```
    ┌─────────────────────┐
    │   Inicialização     │
    │   LCD I2C           │
    └──────────┬──────────┘
               │
               ▼
    ┌─────────────────────┐
    │   Loop Infinito     │
    └──────────┬──────────┘
               │
               ▼
    ┌─────────────────────┐
    │ Aguarda Mutex       │
    │ (Timeout 1s)        │
    └──────────┬──────────┘
               │
               ▼
    ┌─────────────────────┐
    │ Copia Dados da      │
    │ Estrutura mpu6050   │
    └──────────┬──────────┘
               │
               ▼
    ┌─────────────────────┐
    │ Formata Strings     │
    │ sprintf/snprintf    │
    └──────────┬──────────┘
               │
               ▼
    ┌─────────────────────┐
    │ Libera Mutex        │
    └──────────┬──────────┘
               │
               ▼
    ┌─────────────────────┐
    │ Atualiza Display    │
    │ LCD (4 linhas)      │
    └──────────┬──────────┘
               │
               ▼
    ┌─────────────────────┐
    │ Delay 1000ms        │
    └──────────┬──────────┘
               │
               └────────────► (Retorna ao loop)
```

---

## 🔐 Sincronização e Proteção de Dados

### Estrutura de Dados Compartilhada

```cpp
typedef struct {
  float temp;     // Temperatura (°C)
  float accX;     // Aceleração eixo X (m/s²)
  float accY;     // Aceleração eixo Y (m/s²)
  float accZ;     // Aceleração eixo Z (m/s²)
  float gyroX;    // Velocidade angular X (rad/s)
  float gyroY;    // Velocidade angular Y (rad/s)
  float gyroZ;    // Velocidade angular Z (rad/s)
} MPU6050;

MPU6050 mpu6050;  // Variável global compartilhada
```

### Problema de Concorrência

**Sem proteção**, poderíamos ter **race conditions**:

```
Tempo    Task 1 (Escritor)         Memória        Task 2 (Leitor)
─────────────────────────────────────────────────────────────────
t0       Lê sensor: Temp = 25.5°C   
t1       Escreve temp = 25.5        temp = 25.5
t2                                  temp = 25.5    Lê temp = 25.5 ✓
t3       Lê sensor: AccX = 1.2      temp = 25.5
t4       Escreve accX = 1.2         accX = 1.2     Lê accX = ???  ❌
t5                                  accY = OLD     Lê accY = OLD  ❌
```

### Solução com Mutex

```cpp
// Criação do mutex (no setup)
xMutexMPU6050 = xSemaphoreCreateMutex();

// Task 1: Escritor
if (xSemaphoreTake(xMutexMPU6050, timeOut) == pdPASS) {
    // REGIÃO CRÍTICA - Acesso exclusivo garantido
    mpu6050.temp = temp.temperature;
    mpu6050.accX = accel.acceleration.x;
    // ... outros dados
    xSemaphoreGive(xMutexMPU6050);  // Libera
}

// Task 2: Leitor
if (xSemaphoreTake(xMutexMPU6050, timeOut) == pdPASS) {
    // REGIÃO CRÍTICA - Dados consistentes
    snprintf(line1, 21, "Temp: %.1f C", mpu6050.temp);
    snprintf(line2, 21, "ACC: X%.1f Y%.1f Z%.1f", 
             mpu6050.accX, mpu6050.accY, mpu6050.accZ);
    xSemaphoreGive(xMutexMPU6050);  // Libera
}
```

### Timeout de 1 Segundo

```cpp
TickType_t timeOut = pdMS_TO_TICKS(1000);  // 1000ms = 1s
```

Se uma task não conseguir adquirir o mutex em 1 segundo, ela **não bloqueia indefinidamente** e pode tomar ações corretivas.

### 🧪 Procedimento de Teste

1. **Verificação de Endereços I2C**
   ```bash
   # Use o I2C Scanner para verificar
   # MPU6050: 0x68
   # LCD: 0x27
   ```

2. **Upload do Código**
   - Conecte a ESP32 via USB
   - Selecione a placa e porta corretas
   - Faça o upload do código

3. **Monitoramento Serial**
   - Abra o Serial Monitor (115200 baud)
   - Verifique mensagens de inicialização

4. **Teste de Funcionamento**
   - Movimente o sensor MPU6050
   - Observe as mudanças no LCD
   - Valores de aceleração devem mudar

---

## 💻 Código Fonte Comentado

### Setup - Inicialização do Sistema

```cpp
void setup() {
  // Inicializa comunicação I2C
  Wire.begin();
  
  // Inicializa porta serial para debug
  Serial.begin(115200);

  // Cria o mutex para proteção da estrutura mpu6050
  xMutexMPU6050 = xSemaphoreCreateMutex();
  if (xMutexMPU6050 == NULL) {
    Serial.println("Erro ao criar o Mutex!");
    return;  // Sistema não pode continuar sem mutex
  }

  // Task 1: Leitura do MPU6050
  // - Core 1 (núcleo dedicado)
  // - Alta prioridade (configMAX_PRIORITIES - 2)
  // - Stack de 4096 bytes
  xTaskCreatePinnedToCore(
    mpu6050Task,              // Função da task
    "MPU6050_Reader",         // Nome para debug
    4096,                     // Tamanho da stack
    NULL,                     // Sem parâmetros
    configMAX_PRIORITIES - 2, // Prioridade alta
    NULL,                     // Sem handle
    1                         // Core 1
  );
  
  // Delay para garantir inicialização do sensor
  vTaskDelay(pdMS_TO_TICKS(1500)); 
  
  // Task 2: Atualização do LCD
  // - Core 0 (núcleo secundário)
  // - Baixa prioridade (tskIDLE_PRIORITY + 1)
  // - Stack de 4096 bytes
  xTaskCreatePinnedToCore(
    lcdTask,                  // Função da task
    "LCD_Display",            // Nome para debug
    4096,                     // Tamanho da stack
    NULL,                     // Sem parâmetros
    tskIDLE_PRIORITY + 1,     // Prioridade baixa
    NULL,                     // Sem handle
    0                         // Core 0
  );
}
```

### Task 1: Leitura do Sensor

```cpp
void mpu6050Task(void *pvParam) {
  // Inicializa o sensor MPU6050
  if (!mpu.begin()) {
    Serial.println("Falha ao encontrar o chip MPU6050. I2C?");
    vTaskDelay(pdMS_TO_TICKS(5000));
    vTaskSuspend(NULL);  // Suspende a task se falhar
  }

  // Obtém os objetos de sensores individuais
  mpu_temp = mpu.getTemperatureSensor();
  mpu_accel = mpu.getAccelerometerSensor();
  mpu_gyro = mpu.getGyroSensor();

  // Estruturas para armazenar eventos dos sensores
  sensors_event_t accel, gyro, temp;

  while (1) {  // Loop infinito da task
    // Tenta adquirir o mutex (timeout de 1s)
    if (xSemaphoreTake(xMutexMPU6050, timeOut) == pdPASS) {
      // === REGIÃO CRÍTICA (MUTEX ADQUIRIDO) ===
      
      // Lê os dados dos sensores
      mpu_temp->getEvent(&temp);
      mpu_accel->getEvent(&accel);
      mpu_gyro->getEvent(&gyro);

      // Atualiza estrutura compartilhada
      mpu6050.temp = temp.temperature;
      mpu6050.accX = accel.acceleration.x;
      mpu6050.accY = accel.acceleration.y;
      mpu6050.accZ = accel.acceleration.z;
      mpu6050.gyroX = gyro.gyro.x;
      mpu6050.gyroY = gyro.gyro.y;
      mpu6050.gyroZ = gyro.gyro.z;

      // Libera o mutex
      xSemaphoreGive(xMutexMPU6050);
      
      // === FIM DA REGIÃO CRÍTICA ===
    }
    
    // Aguarda 500ms antes da próxima leitura
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}
```

### Task 2: Atualização do Display

```cpp
void lcdTask(void *ptParam) {
  // Inicializa o LCD I2C
  lcd.init();
  lcd.backlight();

  // Buffers para as 4 linhas do display (20 caracteres + '\0')
  const byte lcdChar = 20;
  char line0[lcdChar + 1], line1[lcdChar + 1], 
       line2[lcdChar + 1], line3[lcdChar + 1];
  char *line[] = {line0, line1, line2, line3};

  while (1) {  // Loop infinito da task
    // Tenta adquirir o mutex (timeout de 1s)
    if (xSemaphoreTake(xMutexMPU6050, timeOut) == pdPASS) {
      // === REGIÃO CRÍTICA (MUTEX ADQUIRIDO) ===
      
      // Formata linha 0: Título + Uptime
      sprintf(line0, "MPU6050 T:%d s", xTaskGetTickCount() / 1000);
      
      // Formata linha 1: Temperatura
      snprintf(line1, lcdChar + 1, "Temp: %.1f C", mpu6050.temp);
      
      // Formata linha 2: Acelerômetro
      snprintf(line2, lcdChar + 1, "ACC: X%.1f Y%.1f Z%.1f", 
               mpu6050.accX, mpu6050.accY, mpu6050.accZ);
      
      // Formata linha 3: Giroscópio
      snprintf(line3, lcdChar + 1, "GYRO: X%.1f Y%.1f Z%.1f", 
               mpu6050.gyroX, mpu6050.gyroY, mpu6050.gyroZ);

      // Libera o mutex
      xSemaphoreGive(xMutexMPU6050);
      
      // === FIM DA REGIÃO CRÍTICA ===
    }

    // Atualiza o display LCD (fora da região crítica)
    for (int i = 0; i < 4; i++) {
      lcd.setCursor(0, i);
      lcd.print(line[i]);
    }
    
    // Aguarda 1 segundo antes da próxima atualização
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
```

### Loop Principal

```cpp
void loop() {
  // O loop principal fica praticamente vazio
  // Todo o processamento é feito pelas tasks do FreeRTOS
  vTaskDelay(pdMS_TO_TICKS(100));  // Delay mínimo
}
```

---

## 🖼️ Imagens e Vídeos do Projeto

### 📹 Vídeo de Demonstração


https://github.com/user-attachments/assets/3949fd68-5e14-4b22-b967-8f1bdeb37efe


---

### 📸 Imagens da Montagem

#### Circuito Completo

<img width="676" height="615" alt="Screenshot_2" src="https://github.com/user-attachments/assets/7497a8a1-76dd-4b6b-838e-3b19a450e74c" />


---

## 🔌 Montagem e Testes

### Esquema de Conexões

```
ESP32                  MPU6050
─────                  ───────
3.3V ────────────────► VCC
GND  ────────────────► GND
GPIO 21 (SDA) ───────► SDA
GPIO 22 (SCL) ───────► SCL

ESP32                  LCD 20x4 I2C
─────                  ───────────
5V  ─────────────────► VCC
GND ─────────────────► GND
GPIO 21 (SDA) ───────► SDA
GPIO 22 (SCL) ───────► SCL
```

### Diagrama Fritzing

```
                   ┌───────────────┐
                   │    ESP32      │
                   │   DevKit V1   │
                   │               │
     3.3V ◄────────┤ 3V3       GND ├────────► GND
      GND ◄────────┤ GND           │
                   │               │
                   │    I2C Bus    │
      SDA ◄────────┤ GPIO 21   5V  ├────────► 5V
      SCL ◄────────┤ GPIO 22       │
                   └───────────────┘
                          │
              ┌───────────┴───────────┐
              │                       │
      ┌───────▼─────┐         ┌───────▼─────┐
      │   MPU6050   │         │ LCD 20x4 I2C│
      │   (0x68)    │         │   (0x27)    │
      │             │         │             │
      │ VCC GND     │         │ VCC GND     │
      │ SDA SCL     │         │ SDA SCL     │
      └─────────────┘         └─────────────┘
```

### Simulação no Wokwi

#### Arquivo `diagram.json`

```json
{
  "version": 1,
  "author": "Seu Nome",
  "editor": "wokwi",
  "parts": [
    {
      "type": "wokwi-esp32-devkit-v1",
      "id": "esp",
      "top": 0,
      "left": 0,
      "attrs": {}
    },
    {
      "type": "wokwi-mpu6050",
      "id": "mpu",
      "top": -50,
      "left": 200,
      "attrs": {}
    },
    {
      "type": "wokwi-lcd2004",
      "id": "lcd",
      "top": 150,
      "left": 200,
      "attrs": { "pins": "i2c" }
    }
  ],
  "connections": [
    ["esp:3V3", "mpu:VCC", "red", ["v
