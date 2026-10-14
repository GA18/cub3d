# Fluxo do parser do cub3d

Este documento explica o parser que existe atualmente no projeto. A ideia nao
e apenas dizer "qual funcao chama qual", mas mostrar:

- como uma execucao percorre o parser do inicio ao fim;
- o que cada arquivo e cada funcao fazem;
- o significado das variaveis mais importantes;
- como os loops percorrem o arquivo e o mapa;
- por que cada decisao de fluxo existe;
- onde cada memoria e liberada;
- quais pontos costumam causar mais dificuldade para estudantes de C.

Os arquivos principais do parser sao:

```text
core/main.c
core/src/parser/parser_controller.c
core/src/parser/read_archive.c
core/src/parser/parse_config.c
core/src/parser/parse_color.c
core/src/parser/map_cells.c
core/src/parser/valid_map.c
core/src/parser/valid_routes.c
includes/private/pr_parser.h
includes/public/pu_parser.h
```

## 1. O formato esperado do arquivo `.cub`

O parser trabalha com dois grupos de informacao:

1. configuracoes;
2. mapa.

Um arquivo normalmente possui esta forma:

```text
NO ./textures/north.xpm
SO ./textures/south.xpm
WE ./textures/west.xpm
EA ./textures/east.xpm
F 220,100,0
C 225,30,0
111111
1000N1
111111
```

As configuracoes informam:

| Identificador | Significado | Destino em `t_data` |
| --- | --- | --- |
| `NO` | textura norte | `north_texture` |
| `SO` | textura sul | `south_texture` |
| `WE` | textura oeste | `west_texture` |
| `EA` | textura leste | `east_texture` |
| `F` | cor do piso | `floor` |
| `C` | cor do teto | `ceiling` |

O mapa usa:

| Caractere | Significado |
| --- | --- |
| `0` | espaco caminhavel |
| `1` | parede |
| `N`, `S`, `E`, `W` | jogador e sua direcao inicial |
| espaco | area vazia fora do mapa ou separacao interna |

O parser primeiro le e armazena tudo. Depois valida se o resultado faz
sentido. Essa separacao e importante: ler uma informacao e decidir se ela e
valida sao responsabilidades diferentes.

## 2. A estrutura `t_data`

A estrutura esta declarada em `includes/private/pr_parser.h`:

```c
typedef struct s_data
{
	char	*north_texture;
	char	*south_texture;
	char	*west_texture;
	char	*east_texture;
	t_rgb	floor;
	t_rgb	ceiling;
	int		floor_set;
	int		ceiling_set;
	char	**map;
	int		map_height;
	int		map_width;
	int		player_x;
	int		player_y;
	char	player_dir;
}	t_data;
```

Traduzindo os nomes:

- `north_texture`: caminho da textura norte;
- `south_texture`: caminho da textura sul;
- `west_texture`: caminho da textura oeste;
- `east_texture`: caminho da textura leste;
- `floor`: componentes RGB do piso;
- `ceiling`: componentes RGB do teto;
- `floor_set`: indica se uma linha `F` foi lida;
- `ceiling_set`: indica se uma linha `C` foi lida;
- `map`: vetor de strings, uma string por linha do mapa;
- `map_height`: quantidade de linhas armazenadas;
- `map_width`: maior comprimento encontrado entre as linhas;
- `player_x`: coluna do jogador;
- `player_y`: linha do jogador;
- `player_dir`: caractere original do jogador.

`map` tem formato de vetor terminado por `NULL`:

```text
map[0] -> "111111"
map[1] -> "1000N1"
map[2] -> "111111"
map[3] -> NULL
```

O `NULL` final permite percorrer o vetor sem guardar outro contador, mas o
parser tambem guarda `map_height` porque precisa acessar linhas por indice e
verificar limites.

## 3. Entrada do programa: `main`

Arquivo: `core/main.c`.

```c
int	main(int ac, char **av)
{
	t_data	data;

	if (ac != 2)
		return (1);
	ft_bzero(&data, sizeof(data));
	if (!parser_controller(av[1], &data))
		return (free_data(&data), 1);
	free_data(&data);
	return (0);
}
```

### `ac` e `av`

- `ac` significa argument count: quantidade de argumentos;
- `av` significa argument vector: vetor de strings com os argumentos.

O programa espera exatamente:

```text
./cub3d mapa.cub
```

Nesse caso:

```text
ac == 2
av[0] == "./cub3d"
av[1] == "mapa.cub"
```

Se a quantidade for diferente de dois, o programa encerra imediatamente,
porque nao existe um caminho confiavel para o arquivo a ser lido.

### Por que chamar `ft_bzero`?

`data` e uma variavel local, portanto foi criada na stack. Os campos dela
comecam com valores indefinidos. Antes de usar a estrutura, o codigo zera
todos os bytes:

```c
ft_bzero(&data, sizeof(data));
```

Isso faz com que:

- ponteiros comecem como `NULL`;
- contadores comecem como `0`;
- flags comecem como `0`;
- `player_dir` comece como `'\0'`.

Esse passo e essencial para que `free_data` possa ser chamado mesmo se o
parser falhar no primeiro arquivo ou antes de alocar qualquer coisa.

### O caminho de erro

```c
if (!parser_controller(av[1], &data))
	return (free_data(&data), 1);
```

Se qualquer etapa falhar, o programa chama `free_data` antes de sair. O
parser pode ter lido metade do arquivo e alocado algumas texturas ou linhas
do mapa. Por isso, o caminho de erro precisa liberar o estado parcial.

### O caminho de sucesso

Mesmo depois de uma validacao bem-sucedida, a memoria ainda pertence ao
programa. O `main` chama `free_data` antes de terminar.

Validar uma estrutura nao significa que a memoria foi liberada. Sucesso
tambem precisa de limpeza.

## 4. `parser_controller`: o coordenador

Arquivo: `core/src/parser/parser_controller.c`.

```c
int	parser_controller(const char *path, t_data *data)
{
	if (!path || !data || !split_archive(path, data))
		return (0);
	if (!valid_texture(data))
		return (0);
	if (!valid_map(data))
		return (0);
	return (1);
}
```

O controlador nao interpreta detalhes. Ele organiza as etapas:

```text
1. ler o arquivo;
2. verificar se as configuracoes obrigatorias existem;
3. verificar se o mapa e valido;
4. retornar sucesso ou falha.
```

### Por que validar nessa ordem?

`valid_texture` depende das configuracoes terem sido lidas. `valid_map`
depende do mapa ter sido montado. Portanto, primeiro e necessario executar
`split_archive`.

A validacao do mapa fica depois da textura porque o arquivo inteiro ja foi
carregado e porque um parser costuma separar:

- leitura;
- validacao da configuracao;
- validacao do mapa.

A funcao retorna `1` ou `0`:

- `1`: parser aceitou os dados;
- `0`: alguma etapa rejeitou os dados.

## 5. `split_archive`: leitura completa do arquivo

Arquivo: `core/src/parser/read_archive.c`.

```c
int	split_archive(const char *path, t_data *data)
```

Traducao dos parametros:

- `path`: caminho do arquivo `.cub`;
- `data`: estrutura que recebera os dados lidos.

Variaveis locais:

| Variavel | Significado |
| --- | --- |
| `fd` | file descriptor, identificador inteiro do arquivo aberto |
| `in_map` | indica se o parser ja entrou na regiao do mapa |
| `line` | linha retornada por `get_next_line` |

### Verificacao da extensao

```c
if (!path || ft_strlen(path) < 5
	|| ft_strncmp(path + ft_strlen(path) - 4, ".cub", 4))
	return (0);
```

A funcao rejeita:

- caminho nulo;
- caminho curto demais para conter um nome e `.cub`;
- caminho que nao termina com `.cub`.

`path + ft_strlen(path) - 4` aponta para os quatro ultimos caracteres.
Depois `ft_strncmp` compara esses caracteres com `.cub`.

Esse teste verifica apenas o nome/extensao. Ele nao verifica se as texturas
existem e tambem nao abre o mapa ainda. A abertura acontece no passo seguinte.

### Abertura do arquivo

```c
fd = open(path, O_RDONLY);
if (fd < 0)
	return (0);
```

`open` retorna:

- um descritor nao negativo em caso de sucesso;
- `-1` em caso de falha.

`O_RDONLY` significa que o arquivo sera aberto apenas para leitura.

O `fd` precisa ser fechado com `close(fd)` em todos os caminhos depois que
`open` funcionar. Esquecer isso causa vazamento de descritores, que e um
recurso do sistema diferente de memoria heap.

### Estado inicial da leitura

```c
in_map = 0;
line = get_next_line(fd);
```

No inicio, o parser ainda esta lendo configuracoes. `in_map == 0`
significa:

```text
ainda e permitido interpretar a linha como NO, SO, WE, EA, F ou C.
```

A primeira chamada a `get_next_line` fica fora do `while` de proposito. Alem
de deixar a leitura mais explicita, isso evita colocar uma atribuicao dentro
da condicao do loop.

### Loop principal

```c
while (line)
{
	if (!process_line(line, data, &in_map))
		return (close(fd), 0);
	line = get_next_line(fd);
}
```

O loop continua enquanto `line` for diferente de `NULL`.

`get_next_line` retorna:

- uma nova string alocada quando existe uma linha;
- `NULL` quando chega ao fim do arquivo ou ocorre erro.

Cada iteracao:

1. processa a linha atual;
2. libera ou armazena a linha;
3. busca a proxima linha.

O ponteiro `line` e atualizado no final para que a proxima iteracao trabalhe
com uma nova alocacao.

### Fechamento normal

```c
close(fd);
return (data->map_height > 0);
```

Quando o loop termina, o arquivo e fechado. A funcao considera que houve uma
leitura util se pelo menos uma linha de mapa foi armazenada.

A validacao completa ainda sera feita por `valid_map`. Aqui a pergunta e
somente:

```text
foi encontrada alguma regiao de mapa?
```

### Caminho de erro durante o loop

```c
if (!process_line(line, data, &in_map))
	return (close(fd), 0);
```

Se uma linha invalida for encontrada ou uma alocacao falhar:

1. o descritor e fechado;
2. `split_archive` retorna `0`;
3. `parser_controller` falha;
4. `main` chama `free_data` para limpar o que ja foi alocado.

O `close` aqui nao substitui `free_data`: eles limpam recursos diferentes.

## 6. `process_line`: decidir o destino de cada linha

Arquivo: `core/src/parser/read_archive.c`.

```c
static int	process_line(char *line, t_data *data, int *in_map)
```

Parametros:

- `line`: string original retornada por `get_next_line`;
- `data`: estado acumulado do parser;
- `in_map`: ponteiro para o estado que indica se o mapa comecou.

O uso de `int *in_map` permite que a funcao altere o valor da funcao
chamadora. Se fosse passado apenas `int in_map`, a alteracao seria feita
somente em uma copia.

### Criacao da versao temporaria

```c
trimmed = trim_line(ft_strdup(line));
```

Aqui existem duas strings envolvidas:

1. `line`, pertencente ao loop principal;
2. `ft_strdup(line)`, uma copia temporaria.

`trim_line` chama `ft_strtrim` e libera a copia recebida. Portanto:

```text
line original       continua existindo;
duplicata           e transformada e depois liberada por trim_line;
trimmed             recebe uma nova string com o resultado.
```

A copia e necessaria porque a linha original deve ser preservada para o caso
de ela pertencer ao mapa. O mapa armazena a linha original, nao a linha
aparada.

### Decidir se o mapa comecou

```c
*in_map = is_map_line(trimmed, *in_map);
```

A funcao recebe o estado anterior e a linha aparada. Depois da chamada,
`*in_map` representa o novo estado.

O estado e intencionalmente permanente:

```text
configuracoes -> mapa
```

Depois que o parser entrou no mapa, ele nunca volta para configuracoes. Isso
evita que uma linha posterior do mapa seja interpretada como uma textura.

### Quando a linha pertence ao mapa

```c
if (*in_map)
{
	free(trimmed);
	if (!append_map_line(data, map_line(line)))
		return (free(line), 0);
	return (1);
}
```

Nesse caminho:

1. `trimmed` nao e mais necessario e e liberada;
2. `map_line(line)` remove o `\n` final da string original;
3. `append_map_line` coloca essa string no vetor `data->map`;
4. em caso de sucesso, o mapa assume a posse de `line`;
5. em caso de falha, `line` ainda nao foi armazenada, entao deve ser liberada.

A regra de ownership e:

```text
append_map_line deu sucesso -> line ficou pertencente a data->map
append_map_line falhou     -> process_line ainda deve liberar line
```

Essa regra evita tanto vazamento quanto double free.

### Quando a linha ainda e configuracao

```c
if (trimmed && *trimmed && !parse_config(trimmed, data))
	return (free(trimmed), free(line), 0);
```

As duas primeiras condicoes evitam chamar `parse_config` para:

- `trimmed == NULL`, que significa falha de alocacao;
- string vazia.

Se houver conteudo e `parse_config` rejeitar a linha:

- `trimmed` e liberada;
- `line` e liberada;
- o erro sobe para o controlador.

Se a configuracao for aceita, o codigo chega ao final:

```c
free(trimmed);
free(line);
return (1);
```

A linha de configuracao nunca e armazenada. Ela serviu apenas como entrada
temporaria para preencher `data`.

## 7. `map_line`: remover apenas o newline

```c
static char	*map_line(char *line)
{
	size_t	len;

	len = ft_strlen(line);
	if (len > 0 && line[len - 1] == '\n')
		line[len - 1] = '\0';
	return (line);
}
```

`get_next_line` normalmente retorna linhas terminadas por `\n`. O mapa nao
deve guardar esse caractere como parte da celula, entao a funcao troca:

```text
"1000N1\n" -> "1000N1\0"
```

Ela nao cria memoria nova. A propria string recebida e modificada e devolvida.

Por isso, quem chamou `map_line` continua responsavel pela mesma alocacao.

## 8. `is_map_line`: detectar o inicio do mapa

```c
static int	is_map_line(char *line, int in_map)
{
	return (in_map || (line && *line
			&& (ft_isdigit(*line) || *line == ' ')));
}
```

A decisao possui duas partes.

### Parte 1: mapa ja comecou

```c
in_map
```

Se ja estamos no mapa, qualquer linha seguinte continua sendo mapa,
inclusive uma linha vazia ou uma linha composta por espacos.

Isso e importante porque o mapa pode ter linhas com tamanhos diferentes e
espacos significativos.

### Parte 2: detectar a primeira linha

```c
line && *line
	&& (ft_isdigit(*line) || *line == ' ')
```

O `line &&` impede dereferenciar um ponteiro nulo. O `*line` impede aceitar
uma string vazia. Depois o primeiro caractere precisa ser um digito ou um
espaco.

Na pratica, como `trim_line` remove espacos das pontas, uma linha normal do
mapa que comeca com `1` ou `0` e identificada pelo primeiro digito. Linhas
com espacos internos continuam sendo armazenadas na versao original.

## 9. `append_map_line`: crescer o vetor do mapa

```c
static int	append_map_line(t_data *data, char *line)
```

Variaveis:

- `new_map`: resultado do `realloc`;
- `index`: comprimento da linha atual.

### `realloc`

```c
new_map = realloc(data->map, sizeof(char *) * (data->map_height + 2));
```

O vetor precisa de espaco para:

1. a nova linha;
2. o `NULL` final.

Se `map_height` vale `3`, por exemplo, o novo vetor precisa comportar:

```text
map[0], map[1], map[2], map[3] = NULL
```

Por isso o tamanho e `map_height + 2` ponteiros.

### O erro comum com `realloc`

O resultado deve ser guardado em um ponteiro temporario:

```c
new_map = realloc(data->map, novo_tamanho);
if (!new_map)
	return (0);
data->map = new_map;
```

Se fosse feito diretamente:

```c
data->map = realloc(data->map, novo_tamanho);
```

e o `realloc` falhasse, o endereco antigo poderia ser perdido, causando
vazamento e deixando o estado inutilizavel.

### Armazenar a linha

```c
data->map[data->map_height++] = line;
data->map[data->map_height] = NULL;
```

A linha ocupa o indice atual. Depois `map_height` e incrementado. O `NULL`
e escrito no novo final.

### Atualizar a maior largura

```c
index = (int)ft_strlen(line);
if (index > data->map_width)
	data->map_width = index;
```

O mapa e irregular: cada linha pode ter um comprimento diferente. `map_width`
guarda a maior largura encontrada para que a validacao possa percorrer uma
grade retangular virtual.

Quando uma coordenada ultrapassa o tamanho real de uma linha, `map_at`
considera que existe um espaco vazio naquela posicao.

## 10. `trim_line`: normalizar uma linha temporaria

Arquivo: `core/src/parser/parse_config.c`.

```c
char	*trim_line(char *line)
{
	char	*trimmed;

	trimmed = ft_strtrim(line, " \t\n");
	free(line);
	return (trimmed);
}
```

Essa funcao tem uma responsabilidade de ownership muito importante:

```text
recebe a string line
cria o resultado aparado
libera line
retorna o resultado
```

Quem chama nao pode fazer isto:

```c
trimmed = trim_line(line);
free(line);
```

porque `line` ja foi liberada dentro de `trim_line`. Isso seria double free.

Tambem e importante entender que `ft_strtrim` remove caracteres nas pontas,
nao no meio. O conjunto `" \t\n"` significa:

- espaco;
- tab;
- newline.

## 11. `parse_config`: escolher o tipo da configuracao

Arquivo: `core/src/parser/parse_config.c`.

```c
int	parse_config(char *line, t_data *data)
{
	if (texture_destination(line, data))
		return (parse_texture_config(line, data));
	if (!ft_strncmp(line, "F ", 2) || !ft_strncmp(line, "F\t", 2)
		|| !ft_strncmp(line, "C ", 2) || !ft_strncmp(line, "C\t", 2))
		return (parse_color_config(line, data));
	return (0);
}
```

O fluxo e:

```text
1. a linha e uma textura?
2. se nao, e uma cor F ou C?
3. se nao, a linha e invalida.
```

O teste aceita separacao por espaco ou tab:

```text
NO caminho
NO    caminho
```

A funcao retorna imediatamente depois de delegar o parsing. Isso evita que
uma linha de textura tambem seja testada como cor.

## 12. `texture_destination`: retornar o endereco correto

```c
static char	**texture_destination(char *line, t_data *data)
```

Essa funcao nao retorna o caminho. Ela retorna o endereco do campo que deve
receber o caminho.

Por exemplo, para uma linha `NO`:

```c
return (&data->north_texture);
```

O tipo e `char **` porque:

- `data->north_texture` e `char *`;
- `&data->north_texture` e `char **`.

Isso permite que `parse_texture_config` use uma unica implementacao para os
quatro identificadores:

```c
*destination = value;
```

Sem esse ponteiro para ponteiro, seria necessario repetir quatro blocos:

```c
data->north_texture = value;
data->south_texture = value;
...
```

Se o identificador nao for conhecido, a funcao retorna `NULL`.

## 13. `parse_texture_config`: ler caminhos de texturas

```c
static int	parse_texture_config(char *line, t_data *data)
```

Primeiro, a funcao encontra o destino:

```c
destination = texture_destination(line, data);
```

Depois rejeita dois casos:

```c
if (!destination || *destination)
	return (0);
```

- `!destination`: identificador desconhecido;
- `*destination`: a textura ja foi declarada antes.

A verificacao de `*destination` evita vazamento. Sem ela, uma segunda linha
`NO` poderia substituir o primeiro ponteiro e perder a primeira alocacao.

### Extrair o valor

```c
value = trim_line(ft_substr(line, 3, ft_strlen(line) - 3));
```

Para `NO ./textures/north.xpm`, o prefixo ocupa tres caracteres:

```text
N O espaco
```

A substring comeca no indice `3`, depois e aparada.

Se a alocacao falhar ou o resultado ficar vazio:

```c
if (!value || !*value)
	return (free(value), 0);
```

`free(NULL)` e seguro em C, por isso essa forma tambem cobre falha de
alocacao.

No sucesso, `value` passa a pertencer a `data`:

```c
*destination = value;
```

A funcao nao deve liberar `value` depois disso. A liberacao sera feita por
`free_data`.

## 14. `parse_color_config`: ler `F` e `C`

```c
static int	parse_color_config(char *line, t_data *data)
```

A funcao inicialmente assume piso:

```c
color = &data->floor;
is_set = &data->floor_set;
```

Se o identificador for `C`, ela muda os dois enderecos para o teto:

```c
color = &data->ceiling;
is_set = &data->ceiling_set;
```

Essa e a mesma ideia de `texture_destination`: escolher por ponteiro qual
campo sera preenchido, evitando duplicar o algoritmo.

### Impedir duplicacao

```c
if (*is_set)
	return (0);
```

O valor de `is_set` indica se aquela configuracao ja foi aceita. A regra evita
que uma segunda linha substitua silenciosamente a primeira.

### Extrair e validar a cor

```c
value = trim_line(ft_substr(line, 2, ft_strlen(line) - 2));
if (!value || !parse_color(value, color))
	return (free(value), 0);
```

O prefixo de `F ` ou `C ` tem dois caracteres. A substring remove esse
prefixo, e `parse_color` verifica os tres componentes.

Se der certo:

```c
free(value);
*is_set = 1;
return (1);
```

A string temporaria deixa de ser necessaria depois que seus numeros foram
copiados para `t_rgb`, entao ela e liberada imediatamente.

## 15. `parse_color`: separar uma cor RGB

Arquivo: `core/src/parser/parse_color.c`.

```c
int	parse_color(char *value, t_rgb *color)
```

Entrada:

```text
"220,100,0"
```

Saida:

```text
color->r = 220
color->g = 100
color->b = 0
```

### `ft_split`

```c
parts = ft_split(value, ',');
```

O resultado e um vetor de strings:

```text
parts[0] -> "220"
parts[1] -> "100"
parts[2] -> "0"
parts[3] -> NULL
```

O parser exige exatamente tres partes:

```c
if (!parts || !parts[0] || !parts[1] || !parts[2] || parts[3])
	return (free_parts(parts), 0);
```

Isso rejeita:

- falha de alocacao;
- menos de tres componentes;
- mais de tres componentes.

### Validacao sequencial

```c
ok = parse_rgb_value(parts[0], &color->r);
if (ok)
	ok = parse_rgb_value(parts[1], &color->g);
if (ok)
	ok = parse_rgb_value(parts[2], &color->b);
```

Cada componente so e analisado se o anterior for valido. O retorno de
`parse_rgb_value` fica em `ok`.

Depois, independentemente do resultado:

```c
free_parts(parts);
return (ok);
```

As partes sao temporarias. Elas nao sao armazenadas em `t_data`, entao devem
ser liberadas antes do retorno.

Uma observacao importante: se o primeiro ou segundo componente for valido e
o seguinte falhar, alguns campos de `color` podem ter sido escritos antes da
falha. Como `parse_color_config` so marca `floor_set` ou `ceiling_set` depois
do sucesso, a configuracao continua rejeitada. Os campos escritos sao
inteiros, nao memoria alocada, entao nao existe vazamento nesse caso.

## 16. `parse_rgb_value`: validar um numero

```c
static int	parse_rgb_value(char *value, int *result)
```

Variaveis:

- `value`: ponteiro para o trecho que esta sendo lido;
- `result`: endereco onde o numero validado sera escrito;
- `number`: acumulador numerico;
- `end`: ponteiro usado para verificar o restante da string.

### Ignorar espacos iniciais

```c
while (*value == ' ' || *value == '\t')
	value++;
```

O ponteiro anda pelos espacos sem mover a string original.

### Exigir pelo menos um digito

```c
if (!ft_isdigit(*value))
	return (0);
```

Sem esse teste, uma string vazia ou com letras poderia acabar interpretada
incorretamente.

### Construir o numero

```c
while (ft_isdigit(*value))
{
	number = number * 10 + (*value - '0');
	if (number > 255)
		return (0);
	value++;
}
```

Esse loop implementa a conversao decimal manualmente.

Exemplo com `"220"`:

```text
inicio: number = 0
le '2': number = 0 * 10 + 2  = 2
le '2': number = 2 * 10 + 2  = 22
le '0': number = 22 * 10 + 0 = 220
```

O limite `255` e verificado durante o loop. Isso evita aceitar cores
impossiveis e tambem impede que numeros excessivos continuem crescendo sem
controle.

### Verificar o fim real do componente

```c
end = value;
while (*end == ' ' || *end == '\t')
	end++;
if (*end != '\0')
	return (0);
```

Depois dos digitos, so podem existir espacos ou tabulacoes. Qualquer outra
coisa invalida o componente:

```text
"220"       -> valido
"220   "    -> valido
"220abc"    -> invalido
"220.5"     -> invalido
```

No sucesso:

```c
*result = number;
return (1);
```

O resultado e escrito pelo ponteiro recebido.

## 17. Operacoes basicas das celulas do mapa

Arquivo: `core/src/parser/map_cells.c`.

### `is_player`

```c
int	is_player(char c)
{
	return (c == 'N' || c == 'S' || c == 'E' || c == 'W');
}
```

Retorna verdadeiro quando `c` representa uma direcao inicial do jogador.

### `is_walkable`

```c
int	is_walkable(char c)
{
	return (c == '0' || is_player(c));
}
```

Uma celula caminhavel e:

- `0`;
- a celula onde o jogador esta.

Parede (`1`) nao e caminhavel. Espaco tambem nao e caminhavel.

### `map_at`

```c
char	map_at(t_data *data, int x, int y)
```

Essa funcao acessa uma coordenada sem deixar o restante do codigo lidar com
todos os limites.

```c
if (x < 0 || y < 0 || y >= data->map_height || x >= data->map_width)
	return (' ');
if (x >= (int)ft_strlen(data->map[y]))
	return (' ');
return (data->map[y][x]);
```

Traduzindo:

1. coordenada negativa ou fora da grade virtual retorna espaco;
2. coluna maior que o comprimento real da linha retorna espaco;
3. caso contrario, retorna o caractere armazenado.

O retorno de espaco para fora dos limites e uma decisao de seguranca:
qualquer celula caminhavel que encoste fora do mapa sera rejeitada por
`is_closed_cell`.

### `is_closed_cell`

```c
int	is_closed_cell(t_data *data, int x, int y)
```

Para uma celula caminhavel, a funcao verifica quatro vizinhos:

```text
esquerda
direita
acima
abaixo
```

Cada vizinho precisa ser:

```text
parede ('1') ou caminhavel ('0', 'N', 'S', 'E', 'W')
```

Se um vizinho for espaco, caractere invalido ou estiver fora dos limites,
a funcao retorna falso.

O detalhe mais importante e que a verificacao nao olha diagonais. Ela
garante que cada celula caminhavel tenha os quatro lados protegidos, mas nao
faz uma busca de conectividade completa.

## 18. `valid_map`: validar o mapa inteiro

Arquivo: `core/src/parser/valid_map.c`.

```c
int	valid_map(t_data *data)
```

A funcao possui duas etapas:

1. verificar caracteres e jogador;
2. verificar fechamento das celulas caminhaveis.

### Condicao inicial

```c
if (!data->map || data->map_height < 3 || find_player(data) != 1)
	return (0);
```

O mapa precisa:

- existir;
- possuir pelo menos tres linhas;
- possuir exatamente um jogador.

Menos de tres linhas impossibilita um mapa fechado com interior.

### Segundo loop: percorrer a grade virtual

```c
y = 0;
while (y < data->map_height)
{
	x = 0;
	while (x < data->map_width)
	{
		if (is_walkable(map_at(data, x, y))
			&& !is_closed_cell(data, x, y))
			return (0);
		x++;
	}
	y++;
}
```

O loop externo percorre linhas (`y`). O interno percorre colunas (`x`).

Ele usa `map_width`, a maior largura, e nao o comprimento individual de cada
linha. Por isso `map_at` precisa tratar colunas que nao existem em uma linha
curta como espacos.

A condicao:

```c
is_walkable(cell) && !is_closed_cell(...)
```

significa:

```text
se a celula pode ser percorrida e nao esta fechada, rejeite o mapa.
```

Paredes nao precisam ser verificadas como celulas caminhaveis.

## 19. `find_player` e `inspect_map_line`

`find_player` percorre todas as linhas:

```c
while (y < data->map_height)
{
	line_players = inspect_map_line(data, y);
	if (line_players < 0)
		return (0);
	players += line_players;
	y++;
}
```

### `inspect_map_line`

Essa funcao percorre uma linha caractere por caractere:

```c
while (x < (int)ft_strlen(data->map[y]))
```

Para cada `cell`:

1. verifica se o caractere pertence ao conjunto permitido;
2. verifica se ele e jogador;
3. se for jogador, salva coordenada e direcao;
4. avanca `x`.

Se encontrar algo diferente de `0`, `1`, espaco ou direcao, retorna `-1`.

Se encontrar jogadores, retorna a quantidade daquela linha.

`find_player` soma as quantidades de todas as linhas. No final:

```text
0 jogadores -> invalido
1 jogador   -> valido
2 ou mais   -> invalido
```

Salvar a coordenada a cada jogador e aceitavel porque o mapa so sera aceito
se houver exatamente um. Se houver dois, a funcao falha logo depois e o valor
salvo nao sera usado.

## 20. `valid_texture`

Arquivo: `core/src/parser/valid_routes.c`.

```c
int	valid_texture(t_data *data)
{
	return (data->north_texture && data->south_texture
		&& data->west_texture && data->east_texture
		&& data->floor_set && data->ceiling_set);
}
```

Essa funcao verifica a presenca das seis configuracoes obrigatorias:

- quatro caminhos de textura;
- piso;
- teto.

Os quatro campos de textura sao ponteiros. Um ponteiro nao nulo significa
que uma string foi armazenada.

As flags de cor sao necessarias porque RGB `0,0,0` e uma cor valida. Nao
seria correto usar os valores RGB para decidir se a linha foi lida:

```text
F 0,0,0
```

possui valores todos iguais a zero, mas continua sendo uma configuracao
valida e presente. Por isso existem `floor_set` e `ceiling_set`.

Esta funcao confirma apenas que os valores foram declarados. Ela nao abre os
arquivos de textura para verificar se existem.

## 21. `free_data`: limpeza centralizada

```c
void	free_data(t_data *data)
```

Essa e uma das funcoes mais importantes do parser.

### Protecao contra ponteiro nulo

```c
if (!data)
	return ;
```

Permite chamar `free_data(NULL)` sem acessar memoria invalida.

### Liberar texturas

```c
free(data->north_texture);
free(data->south_texture);
free(data->west_texture);
free(data->east_texture);
```

Cada caminho foi criado por `ft_substr`/`ft_strtrim` durante o parsing e
passou a pertencer a `data`. Portanto, cada um precisa de um `free`.

`free(NULL)` e seguro. Isso permite liberar todos os quatro campos mesmo se
apenas um ou nenhum tiver sido lido.

### Liberar linhas do mapa

```c
i = 0;
while (data->map && data->map[i])
	free(data->map[i++]);
```

Existem dois niveis de memoria:

```text
data->map       -> vetor de ponteiros
data->map[i]    -> cada string de uma linha
```

Primeiro e necessario liberar cada string. Depois o vetor:

```c
free(data->map);
```

Liberar apenas `data->map` perderia todas as strings apontadas por ele.

### Zerar a estrutura

```c
ft_bzero(data, sizeof(*data));
```

Depois da limpeza, os ponteiros voltam a ser `NULL` e os contadores voltam a
zero. Isso torna a funcao segura para ser chamada novamente.

## 22. Mapa de ownership da memoria

A pergunta mais importante para cada alocacao e:

```text
quem e o dono desta memoria agora?
```

### Linhas retornadas por `get_next_line`

```text
get_next_line -> line
```

Se a linha for configuracao:

```text
process_line usa line
process_line libera line
```

Se a linha for mapa e `append_map_line` funcionar:

```text
get_next_line -> process_line -> data->map
free_data -> libera depois
```

Se `append_map_line` falhar:

```text
line nao entrou no mapa
process_line libera line
```

### Copia aparada

```text
ft_strdup(line) -> trim_line
trim_line       -> libera a copia original
trim_line       -> retorna trimmed
process_line    -> libera trimmed
```

Essa copia nunca deve ser confundida com a linha original do mapa.

### Caminho de textura

```text
ft_substr
    |
    v
trim_line
    |
    v
data->north_texture (ou outro campo)
    |
    v
free_data
```

Depois de atribuir `*destination = value`, `parse_texture_config` nao pode
liberar `value`.

### Cor

```text
ft_substr -> value
ft_split(value, ',') -> parts
```

Depois de copiar os numeros:

```text
free_parts(parts)
free(value)
```

Os numeros ficam dentro de `t_rgb`, entao as strings temporarias podem ser
descartadas.

## 23. Os gargalos mais comuns para estudantes

### 23.1 Confundir stack, heap e ponteiro

```c
t_data	data;
```

`data` e uma variavel local na stack. Ja os caminhos e linhas sao alocados
dinamicamente no heap.

```c
char	*value;
value = ft_substr(...);
```

`value` e apenas um ponteiro na stack. A string apontada esta no heap.

Liberar o ponteiro nao significa liberar a variavel local. Significa liberar
a memoria heap para a qual ele aponta.

### 23.2 Liberar a coisa errada depois de transferir ownership

Isto e errado:

```c
*destination = value;
free(value);
```

Depois do `free`, `data` apontaria para memoria invalida.

A regra e:

```text
antes da atribuicao: funcao local e dona
depois da atribuicao: data e dono
```

### 23.3 Double free em `trim_line`

`trim_line` ja libera a string recebida. Portanto:

```c
trimmed = trim_line(copy);
free(copy);
```

e errado.

### 23.4 Esquecer o `NULL` final de `char **`

O vetor do mapa precisa terminar em `NULL`:

```text
map[altura] = NULL
```

Sem isso, `free_data` continuaria lendo memoria alem do vetor:

```c
while (data->map && data->map[i])
```

Isso poderia causar segmentation fault ou free de enderecos invalidos.

### 23.5 Usar `realloc` diretamente no ponteiro original

O padrao correto e:

```c
new_map = realloc(data->map, new_size);
if (!new_map)
	return (0);
data->map = new_map;
```

O temporario preserva o ponteiro antigo caso a realocacao falhe.

### 23.6 Confundir `map_height` e `map_width`

- `map_height`: quantidade de strings;
- `map_width`: maior quantidade de caracteres em uma string.

No loop:

```c
while (y < data->map_height)
while (x < data->map_width)
```

`y` percorre linhas. `x` percorre colunas.

### 23.7 Nao tratar mapas irregulares

Uma linha pode ter menos caracteres que `map_width`. Por isso nao se deve
fazer diretamente:

```c
data->map[y][x]
```

sem saber se `x` existe naquela linha. `map_at` centraliza essa verificacao e
retorna espaco quando a coluna esta fora do comprimento real.

### 23.8 Confundir `0` com `'\0'`

No mapa:

```text
'0'   -> caractere caminhavel
'\0'  -> fim da string
```

Eles nao sao a mesma coisa. O primeiro e o caractere ASCII zero; o segundo e
o terminador de uma string C.

### 23.9 Esquecer que `&&` e `||` fazem curto-circuito

Em:

```c
if (trimmed && *trimmed && !parse_config(trimmed, data))
```

`*trimmed` so e acessado se `trimmed` nao for `NULL`. Essa ordem protege
contra dereferencia de ponteiro nulo.

Em:

```c
is_walkable(cell) && !is_closed_cell(data, x, y)
```

`is_closed_cell` so e chamada se a celula for caminhavel. Isso evita trabalho
desnecessario para paredes.

### 23.10 Confundir descritor de arquivo com `FILE *`

`open` retorna um `int` chamado file descriptor:

```c
int	fd;
```

Ele e fechado com:

```c
close(fd);
```

Isso e diferente da API `fopen`/`fclose`, que usa `FILE *`.

### 23.11 Fechar o arquivo nao libera strings

```c
close(fd);
```

libera o recurso do sistema associado ao arquivo. Nao libera:

- `line`;
- caminhos de textura;
- linhas do mapa;
- vetor `data->map`.

Essas memorias continuam sendo responsabilidade de `free_data` ou do
caminho local de erro.

### 23.12 Retornar cedo sem esquecer limpeza

O parser possui muitos retornos antecipados:

```c
return (0);
```

Retornar cedo e bom para manter o fluxo legivel, mas cada retorno precisa
perguntar:

```text
existe fd aberto?
existe uma string temporaria?
existe memoria parcial em data?
```

No fluxo atual:

- `process_line` limpa a linha que recebeu;
- `split_archive` fecha o `fd`;
- `main` chama `free_data` para limpar `data`.

Essa divisao de responsabilidades evita que todos os niveis tentem liberar
a mesma coisa.

## 24. Resumo visual do fluxo

```text
main
 |
 | ft_bzero(data)
 v
parser_controller
 |
 +--> split_archive
 |      |
 |      +--> valida caminho .cub
 |      +--> open
 |      +--> get_next_line
 |      |      |
 |      |      +--> process_line
 |      |             |
 |      |             +--> ainda em configuracao?
 |      |             |      |
 |      |             |      +--> parse_config
 |      |             |             |
 |      |             |             +--> textura
 |      |             |             +--> parse_color
 |      |             |
 |      |             +--> mapa?
 |      |                    |
 |      |                    +--> map_line
 |      |                    +--> append_map_line
 |      |
 |      +--> close
 |
 +--> valid_texture
 |
 +--> valid_map
        |
        +--> find_player
        |      |
        |      +--> inspect_map_line
        |
        +--> percorrer x/y
               |
               +--> map_at
               +--> is_walkable
               +--> is_closed_cell
 |
 +--> sucesso ou falha
        |
        +--> free_data
```

## 25. Checklist mental para estudar ou depurar

Quando algo falhar, seguir esta ordem:

1. `ac` esta correto?
2. `data` foi zerado antes do parser?
3. `path` termina em `.cub`?
4. `open` retornou um descritor valido?
5. `get_next_line` esta retornando as linhas esperadas?
6. `in_map` mudou no momento certo?
7. uma linha de configuracao foi liberada?
8. uma linha do mapa foi transferida para `data->map`?
9. `realloc` foi guardado em ponteiro temporario?
10. o vetor do mapa termina em `NULL`?
11. as seis configuracoes obrigatorias foram encontradas?
12. existe exatamente um jogador?
13. todos os caracteres sao permitidos?
14. toda celula caminhavel tem quatro vizinhos validos?
15. todos os caminhos de erro fecham o arquivo e liberam a memoria?

O ponto central do parser e este: cada funcao transforma uma parte do texto
em um estado mais estruturado, e cada etapa deve deixar claro quem possui
cada recurso. Se o fluxo e entendido como "ler -> armazenar -> validar ->
liberar", a maior parte dos bugs deixa de ser misteriosa.
