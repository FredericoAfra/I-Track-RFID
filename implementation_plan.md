# Plan de Implementação - Sistema Industrial de Rastreamento de Ativos RFID

Desenvolveremos uma aplicação web de arquivo único (`index.html`) usando HTML5, CSS, JavaScript (Vanilla), Tailwind CSS (via CDN), Firebase JS SDK v8 (compat API) e Chart.js.

A plataforma foi projetada para rastreamento industrial de ativos, apresentando telemetria em tempo real, gerenciamento de tags, configuração de pontos de leitura e histórico de movimentação.

---

## Design System & Estética
- **Tema:** Estilo industrial e profissional de alto contraste. Fundos escuros em tom marinho e ardósia (`#0f172a`, `bg-slate-900`/`bg-slate-800`), combinados com um tom de destaque âmbar/laranja (`#d97706`, `amber-600`) que remete a sinalizações de segurança de pátios industriais.
- **Tipografia:** Fonte moderna sem serifa (Inter/Outfit via Google Fonts).
- **Idioma:** Todos os textos da interface, botões, formulários e gráficos estarão em português do Brasil (pt-BR).
- **Estrutura:** Barra de navegação superior limpa, cartões com bordas arredondadas e sombras suaves, tabelas organizadas e responsivas.

---

## Alterações Propostas

### Frontend

#### [MODIFY] [index.html](file:///d:/projects/IRFIDT/Frontend/index.html)
Substituiremos completamente o arquivo `index.html` básico por uma SPA completa.

O arquivo conterá:
1. **Bibliotecas via CDN**:
   - Tailwind CSS Play CDN: `<script src="https://cdn.tailwindcss.com"></script>`
   - Configurações personalizadas do Tailwind para cores temáticas (slate-900, slate-800, amber-500, etc.).
   - Google Fonts (Inter).
   - FontAwesome CDN (para ícones industriais e de monitoramento).
   - Firebase SDK v8 (App, Auth, Database).
   - Chart.js CDN.
2. **CSS Global**: Estilos para scrollbars personalizadas, efeitos de transição e microanimações interativas.
3. **Estrutura HTML (SPA)**:
   - **Tela de Login**: Tela centralizada com estilo industrial, campos de email e senha, botão de login destacado e alertas de erro em caso de falha de autenticação.
   - **Interface Principal (Pós-login)**:
     - **Navbar Superior**: Logotipo com ícone de ondas RFID, botões de navegação para alternar entre as abas (Dashboard, Gerenciamento de Tags, Pontos de Leitura, Histórico de Tag), identificação do usuário logado e botão de Sair (Logout).
     - **Seção 1 — Dashboard**:
       - 4 cartões com indicadores em tempo real (Total de Leituras, Ativos no Armazém, Pontos Ativos, Última Leitura).
       - Gráficos integrados com Chart.js:
         - **Gráfico A (Doughnut ou Barra)**: Inventário do Armazém, comparando o valor Inicial (de `/config/inventario_inicial`) com o valor Atual (tags onde `posicao === 'armazem'`).
         - **Gráfico B (Barra)**: Leituras por Ponto, agrupando as leituras de `/leituras` por ponto.
     - **Seção 2 — Gerenciamento de Tags**:
       - Tabela com colunas: UID | Nome | Categoria | Posição Atual | Última Leitura | Ações.
       - A última leitura exibirá data/hora formatada de forma legível. A coluna de Ações terá um botão de exclusão que remove a tag após confirmação.
       - Formulário para adicionar tags: UID, Nome, Categoria e Destino Padrão (Select com opções 1 -> Esteira 2, 2 -> Esteira 1, 3 -> Despache).
     - **Seção 3 — Pontos de Leitura**:
       - Tabela com colunas: Nome | Descrição | Tags no Local | Ações.
       - O campo "Tags no Local" calcula dinamicamente a quantidade de tags onde `posicao === nome_do_ponto`.
       - Formulário para adicionar ponto de leitura (Nome sem espaços e em minúsculas, Descrição).
     - **Seção 4 — Histórico de Tag**:
       - Dropdown populado dinamicamente com todas as tags (Nome e UID).
       - Botão "Buscar" que realiza a consulta filtrada em `/leituras` por `tag_id`.
       - Tabela com os resultados ordenados por timestamp decrescente (Data/Hora no padrão pt-BR, Ponto, Destino).
4. **Lógica em Javascript**:
   - Inicialização e configurações do Firebase (com placeholders).
   - Mecanismo SPA (funções que alternam classes de visibilidade entre as seções).
   - Ouvinte de estado de autenticação (`firebase.auth().onAuthStateChanged`).
   - Listeners em tempo real (`.on('value')`) para escutar alterações nas tags, leituras, pontos e inventário inicial.
   - Gerenciamento dos gráficos Chart.js (criação e atualização dinâmica dos dados).
   - Handlers de envio de formulários e remoção de registros do banco de dados Firebase.

---

## Plano de Verificação

### Verificação Manual
1. **Acesso não autenticado**: Garantir que apenas a tela de login seja exibida inicialmente.
2. **Falhas no login**: Testar credenciais incorretas e checar se o alerta é exibido.
3. **Navegação**: Clicar nos links de navegação para alternar entre seções sem recarga de página.
4. **Cadastro de novas Tags / Pontos de Leitura**: Preencher os formulários, submeter e verificar se os dados são salvos no Realtime Database.
5. **Atualizações em tempo real**: Adicionar/remover registros e constatar se as tabelas e os gráficos são re-renderizados instantaneamente.
6. **Consulta de Histórico**: Selecionar uma tag, buscar e ver as leituras em ordem cronológica decrescente.
