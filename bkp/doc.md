# Proposta de Desenvolvimento de Sistema de Aluguel de Carrinhos de Controle Remoto

## 1. Introdução

Esta proposta detalha o desenvolvimento de um sistema para gerenciar e rastrear o aluguel de carrinhos de controle remoto em um quiosque de shopping. O sistema será baseado em NestJS e NextJS, podendo adotar uma arquitetura de microsserviços conforme necessário.

## 2. Escopo do Projeto

### Funcionalidades Principais

- **Controle de Aluguéis**: Registro de início, pausa e fim do aluguel, com cronômetro e histórico detalhado.
- **Gestão de Carrinhos**: Cadastro e monitoramento da disponibilidade dos carrinhos.
- **Registro de Ponto**: Registro de presença dos operadores com verificação por foto.
- **Histórico de Aluguéis**: Dados detalhados sobre uso dos carrinhos para análise.
- **Comunicação ESP32**: Implementação de WebSocket para comunicação com os carrinhos.
- **Gestão de Pagamentos**: Registro de valores pagos, sem integração com gateway de pagamento.

## 3. Arquitetura e Tecnologias

### Tecnologias Utilizadas

- **Frontend**: NextJS
- **Backend**: NestJS com Prisma
- **Banco de Dados**: PostgreSQL
- **Comunicação**: WebSockets para integração com ESP32
- **Infraestrutura**: AWS ou VPS para hospedagem

### Modelagem de Banco de Dados

```prisma
model User {
  id        Int     @id @default(autoincrement())
  name      String
  email     String  @unique
  password  String
  role      String  // 'operator' ou 'admin'
  createdAt DateTime @default(now())
}

model Cart {
  id        Int     @id @default(autoincrement())
  identifier String  @unique
  status     String  // 'available' ou 'rented'
  createdAt DateTime @default(now())
}

model Rental {
  id        Int      @id @default(autoincrement())
  cartId    Int
  userId    Int
  startTime DateTime
  endTime   DateTime?
  status    String   // 'active' ou 'completed'
  cart      Cart     @relation(fields: [cartId], references: [id])
  user      User     @relation(fields: [userId], references: [id])
}

model Attendance {
  id        Int      @id @default(autoincrement())
  userId    Int
  timestamp DateTime
  photo     String  // URL da foto
  user      User     @relation(fields: [userId], references: [id])
}
```

### Rotas de API

- **/users**:
  - `POST`: Criar um novo usuário.
  - `GET`: Listar todos os usuários.
- **/carts**:
  - `POST`: Adicionar um novo carrinho.
  - `GET`: Listar todos os carrinhos.
- **/rentals**:
  - `POST`: Iniciar um novo aluguel.
  - `PATCH /:id`: Atualizar um aluguel (pausar, finalizar).
  - `GET`: Listar todos os aluguéis.
- **/attendance**:
  - `POST`: Registrar ponto de um operador.
  - `GET`: Listar todos os registros de ponto.

## 4. Comunicação

### Opções de Comunicação

1. **Wi-Fi (ESP32 com WebSocket)**
   - **Custo**: ~R$25 por dispositivo.
   - **Vantagem**: Implementação mais simples.
   - **Desvantagem**: Pode sofrer interferências.
2. **LoRa (ESP32 + Módulo LoRa)**
   - **Custo**: ~R$50 por dispositivo.
   - **Vantagem**: Comunicação estável em longas distâncias.
   - **Desvantagem**: Maior complexidade na implementação.

### Custos Estimados

| Solução  | Custo por Carrinho | Total para 10 Carrinhos | Gateway (Raspberry Pi) | Custo Total |
|----------|------------------|----------------------|---------------------|-------------|
| Wi-Fi    | R$25             | R$250                | R$400               | R$650       |
| LoRa     | R$50             | R$500                | R$400               | R$900       |

## 5. Equipe e Prazos

### Equipe

- **Desenvolvedor Sênior**: Arquitetura e backend.
- **Desenvolvedor Júnior**: Frontend e suporte backend.
- **UX Designer**: Interface e experiência do usuário.

### Cronograma

- **Fase 1: Planejamento e Design** (2 semanas)
- **Fase 2: Desenvolvimento Inicial** (4 semanas)
- **Fase 3: Testes e Ajustes** (2 semanas)
- **Fase 4: Implantação e Treinamento** (2 semanas)

## 6. Estimativa de Custos

- **Salários**:
  - Desenvolvedor Sênior: R$X/semana
  - Desenvolvedor Júnior: R$Y/semana
  - UX Designer: R$Z/semana
- **Infraestrutura**:
  - DevOps: R$A/mês
  - Domínio e Servidor: R$B/mês

**Total Estimado**: R$C

## 7. Conclusão

Esta proposta detalha um sistema robusto para gestão de aluguel de carrinhos, utilizando tecnologias modernas e garantindo uma solução escalável e eficiente. A escolha entre comunicação Wi-Fi ou LoRa será feita conforme as necessidades do cliente e as condições do ambiente de uso. Com uma equipe bem estruturada e um cronograma definido, o projeto tem potencial para ser entregue dentro do prazo e orçamento previstos.
