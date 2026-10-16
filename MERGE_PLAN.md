# Plano de integracao do cub3D

## 1. Objetivo

Integrar o parser deste repositorio com o runtime grafico, movimentacao e
raycast da branch `dupla/rewrite`.

O merge nao deve ser feito com `git merge --allow-unrelated-histories`.
Os historicos nao compartilham ancestral e as duas arvores representam
arquiteturas diferentes. A integracao deve ser feita por camadas, em uma
branch de trabalho, mantendo cada etapa compilavel.

## 2. Decisao de arquitetura

Usar o parser atual como fonte de verdade para o arquivo `.cub` e usar o V1
da dupla como base para a camada grafica.

Fluxo final esperado:

```text
main
  -> runtime_args
  -> scene_load
       -> leitura das configuracoes
       -> leitura do mapa
       -> validacao de caracteres
       -> validacao de fechamento
       -> flood fill, se a politica de conectividade for mantida
  -> game_setup
  -> game_loop
       -> input
       -> player_update
       -> raycast
       -> render
  -> scene_free / runtime_cleanup
```

O parser nao deve abrir janela nem depender da MiniLibX. O runtime nao deve
interpretar o arquivo `.cub` diretamente.

## 3. Estrutura alvo

```text
Makefile
MERGE_PLAN.md
includes/
    cub3d.h                 # entrada publica unica do programa
    parser.h                # contrato do parser
    game.h                  # contrato do loop e estado do jogo
    shared/
        enums.h
        constants.h
        structs.h
core/
    main.c
    runtime.c
    setup.c
    common.c
    input/
        hook.c
    game/
        game.c
    player/
        player.c
    render/
        screen.c
        raycast.c
        render_2d.c
        render_3d.c
    parser/
        parser_controller.c
        read_archive.c
        parse_config.c
        parse_color.c
        map_cells.c
        valid_map.c
        map_connectivity.c
        map_connectivity_utils.c
        valid_routes.c
libft/
```

`core/src/parser` pode ser movido para `core/parser` para reduzir a
profundidade dos includes. A mudanca deve ser feita em um unico commit de
organizacao, antes da integracao do runtime.

## 4. Modelo de dados unificado

Hoje existem dois modelos incompatíveis:

- o parser usa `t_data`, `t_rgb`, `char **map`, coordenadas inteiras do
  jogador e direcao em `player_dir`;
- o V1 usa `t_game`, `t_map`, cores inteiras e `t_player` com posicao/direcao
  em ponto flutuante.

Criar um modelo unico, sem manter dois mapas em memoria:

```c
typedef struct s_spawn
{
    int x;
    int y;
    char direction;
} t_spawn;

typedef struct s_scene
{
    char **grid;
    int width;
    int height;
    t_rgb floor;
    t_rgb ceiling;
    char *texture[4];
    t_spawn spawn;
} t_scene;

typedef struct s_game
{
    void *mlx;
    void *win;
    t_scene scene;
    t_img frame;
    t_player player;
    t_view view;
    double last_frame;
    int keys[GK_COUNT];
} t_game;
```

Preferir `t_scene` como nome do antigo `t_data`. O `t_map` do V1 deve ser
removido ou substituido por `t_scene`; manter ambos cria duas fontes de
verdade para largura, altura, grid e texturas.

Regras de ownership:

- `scene_load` aloca `grid` e texturas;
- `game_setup` apenas referencia `g->scene`, sem duplicar strings;
- `scene_free` libera tudo que foi alocado pelo parser;
- `runtime_cleanup` chama `scene_free` depois de destruir imagens e janela;
- qualquer falha parcial deve passar pelo mesmo cleanup.

## 5. Movimentacao de arquivos e funcoes

### Parser atual

| Origem | Destino | Acao |
| --- | --- | --- |
| `core/main.c` | `core/main.c` | substituir o main de teste pelo main grafico |
| `core/src/parser/parser_controller.c` | `core/parser/parser_controller.c` | manter como `scene_load` ou adaptar a assinatura |
| `core/src/parser/read_archive.c` | `core/parser/read_archive.c` | manter a leitura do `.cub` |
| `core/src/parser/parse_config.c` | `core/parser/parse_config.c` | manter, adaptando `t_data` para `t_scene` |
| `core/src/parser/parse_color.c` | `core/parser/parse_color.c` | manter RGB e validacao |
| `core/src/parser/map_cells.c` | `core/parser/map_cells.c` | manter `map_at`, celulas caminhaveis e fechamento |
| `core/src/parser/valid_map.c` | `core/parser/valid_map.c` | manter validacao de jogador e mapa |
| `core/src/parser/map_connectivity.c` | `core/parser/map_connectivity.c` | manter apenas se conectividade for uma regra desejada |
| `core/src/parser/map_connectivity_utils.c` | `core/parser/map_connectivity_utils.c` | mover junto com o flood fill |
| `core/src/parser/valid_routes.c` | `core/parser/valid_routes.c` | dividir em `scene_free.c` e `texture_validation.c` se necessario |
| `includes/private/pr_parser.h` | `includes/parser.h` | absorver tipos e contratos do parser |
| `includes/public/pu_parser.h` | remover | o programa passa a ter `cub3d.h` como header de entrada |

Renomes recomendados:

- `t_data` -> `t_scene`;
- `map` -> `grid`;
- `map_width`/`map_height` -> `width`/`height`;
- `player_x`, `player_y`, `player_dir` -> `spawn.x`, `spawn.y`,
  `spawn.direction`;
- `free_data` -> `scene_free`;
- `valid_map` -> `scene_validate`;
- `parser_controller` -> `scene_load`.

### Runtime e jogo da dupla

| Origem na dupla | Destino | Acao |
| --- | --- | --- |
| `V1/source/main.c` | `core/main.c` | usar como base, chamando `scene_load` |
| `V1/source/runtime.c` | `core/runtime.c` | manter argumentos, loop e cleanup; trocar `map_parse` por `scene_load` |
| `V1/source/setup.c` | `core/setup.c` | manter setup da MLX, hooks e frame |
| `V1/source/common.c` | `core/common.c` | manter teclado e relogio |
| `V1/source/game.c` | `core/game/game.c` | manter `game_frame` |
| `V1/source/hook.c` | `core/input/hook.c` | manter eventos, adaptando tipos |
| `V1/source/player.c` | `core/player/player.c` | manter movimentacao e rotacao, corrigindo eixo Y |
| `V1/source/raycast.c` | `core/render/raycast.c` | manter DDA, usando `scene.grid` |
| `V1/source/screen.c` | `core/render/screen.c` | manter limpeza e apresentacao do frame |
| `V1/source/map.c` | nao copiar como esta | `map_parse` e hardcoded; substituir pela saida do parser |
| `V1/header/shared/struct.h` | `includes/shared/structs.h` | fundir structs com `t_scene` |
| `V1/header/shared/enum.h` | `includes/shared/enums.h` | manter enums de teclas, view e ray side |
| `V1/header/shared/const.h` | `includes/shared/constants.h` | manter constantes de janela, teclas e cores |
| `V1/header/source/*.h` | `includes/` | consolidar por modulo, sem includes duplicados |

## 6. Correcoes obrigatorias antes do merge funcional

O V1 testado nao esta pronto para ser incorporado sem estas correcoes:

1. O Makefile compila apenas `source/main.c`; precisa listar todos os fontes
   ou usar uma lista explicita de fontes.
2. O Makefile nao constroi `libft.a` nem `libmlx.a` antes do link.
3. `source/map.c` ignora o caminho recebido e cria um mapa hardcoded; deve
   ser removido do fluxo e substituido por `scene_load`.
4. `source/screen.c` chama `render_3D` e `render_2D`, mas essas funcoes nao
   existem no V1 atual. Implementar as duas ou retirar as chamadas
   temporariamente com um estado de erro explicito.
5. Em `source/player.c`, o movimento no eixo Y atualiza `pos_x`; corrigir
   para `pos_y`.
6. A colisao deve tratar celulas ausentes/espacos como solidas, em acordo
   com a representacao de mapa irregular do parser.
7. Converter a direcao textual `N/S/E/W` do spawn para `dir_x`, `dir_y`,
   `plane_x` e `plane_y` antes do primeiro frame.
8. Decidir explicitamente se flood fill de conectividade sera requisito.
   O subject exige mapa fechado, mas nao declara que toda area caminhavel
   precisa ser alcancavel pelo jogador.

## 7. Plano de execucao por fases

### Fase 0 - congelar referencias

- Criar branch `merge/integracao-grafica` a partir do parser atual.
- Manter `dupla/rewrite` como remote somente leitura.
- Nao apagar o clone da dupla ainda.
- Registrar o commit usado da dupla: `eee034c`.

### Fase 1 - consolidar tipos e headers

- Criar `includes/cub3d.h` e os headers por modulo.
- Introduzir `t_scene` e `t_game` unificados.
- Remover a duplicacao de `t_point`.
- Fazer parser e runtime compilarem sem MLX em um teste unitario do parser.

### Fase 2 - mover o parser

- Mover os arquivos do parser para `core/parser`.
- Renomear `t_data` e os campos conforme a tabela acima.
- Manter os testes existentes em `.tests`.
- Rodar `make`, `bash .tests/test_maps.sh` e Valgrind antes de adicionar MLX.

### Fase 3 - integrar runtime e estado do jogo

- Mover runtime, setup, common, game, input e player.
- Fazer `runtime_setup` chamar `scene_load`.
- Remover `map_parse` hardcoded.
- Inicializar o jogador a partir de `scene.spawn`.
- Corrigir colisao, movimento Y e cleanup.

### Fase 4 - integrar raycast e render

- Mover DDA para `core/render/raycast.c`.
- Implementar `render_2D` usando a mesma `scene.grid`.
- Implementar `render_3D` usando `raycast_cast`.
- Validar paredes, espacos, limites e mapas irregulares.

### Fase 5 - Makefile e dependencias

- Usar lista explicita de fontes; evitar `wildcard` ate a arvore estabilizar.
- Adicionar targets para `libft` e MLX antes do link.
- Gerar objetos em `build/` sem rastrea-los no Git.
- Manter `all`, `clean`, `fclean`, `re` e `bonus`.
- Remover binarios, objetos e bibliotecas geradas do controle de versao.

### Fase 6 - validacao de integracao

- Parser: todos os mapas validos e invalidos existentes.
- Flood fill: casos `FF_` e politica escolhida para regioes desconectadas.
- Runtime: argumentos invalidos, extensao, arquivo inexistente e cleanup.
- Gameplay: W/A/S/D, setas, ESC, X da janela e troca 2D/3D.
- Raycast: paredes em quatro direcoes, cantos, corredores e mapas grandes.
- Memoria: Valgrind em erro de parser, erro de setup, fechamento de janela e
  encerramento normal.
- Build: `make re` a partir de uma arvore limpa.

## 8. Criterios de pronto

- `make re` termina sem warnings com `-Wall -Wextra -Werror`.
- `./cub3d mapa.cub` usa o mapa recebido, sem dados hardcoded.
- Os testes do parser continuam passando.
- O jogo abre com texturas, cores, spawn e direcao vindos do `.cub`.
- Nenhuma funcao do parser depende da MLX.
- Nenhum recurso alocado pelo parser ou runtime vaza em qualquer caminho de
  erro testado.
- `git status` nao mostra objetos, bibliotecas ou binarios gerados.
- So depois desses criterios o clone da dupla pode ser removido.

## 9. Riscos e decisoes pendentes

- O V1 nao compila atualmente; a integracao deve tratar isso como trabalho de
  reparo, nao como um merge de arquivos pronto.
- O parser atual e mais rigoroso que o subject ao exigir conectividade. A
  decisao deve ser registrada nos testes antes de estabilizar `scene_validate`.
- As texturas ainda sao apenas caminhos no parser; a abertura via MLX precisa
  validar falha e cleanup.
- O formato irregular do mapa exige uma politica unica para colisoes e
  raycast fora do comprimento de uma linha.
- A libft atual deve permanecer como base. Funcoes extras da libft da dupla
  so devem ser portadas quando um modulo realmente precisar delas.