#!/bin/bash

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
PROGRAM="$ROOT_DIR/cub3d"
VALID_DIR="$ROOT_DIR/.tests/valid_maps"
INVALID_DIR="$ROOT_DIR/.tests/invalid_maps"

if [ ! -x "$PROGRAM" ]; then
	printf 'Erro: executavel nao encontrado: %s\n' "$PROGRAM"
	printf 'Execute "make" na raiz do projeto antes dos testes.\n'
	exit 1
fi

valid_total=0
valid_failures=0
invalid_total=0
invalid_failures=0

printf '%s\n' '=== Mapas validos ==='
for file in "$VALID_DIR"/*.cub; do
	[ -f "$file" ] || continue
	valid_total=$((valid_total + 1))
	if "$PROGRAM" "$file" >/dev/null 2>&1; then
		printf 'OK      %s\n' "$(basename "$file")"
	else
		printf 'FALHOU  %s\n' "$(basename "$file")"
		valid_failures=$((valid_failures + 1))
	fi
done

printf '\n%s\n' '=== Mapas invalidos ==='
for file in "$INVALID_DIR"/*.cub "$INVALID_DIR"/*.cube; do
	[ -f "$file" ] || continue
	invalid_total=$((invalid_total + 1))
	if "$PROGRAM" "$file" >/dev/null 2>&1; then
		printf 'ACEITO  %s\n' "$(basename "$file")"
		invalid_failures=$((invalid_failures + 1))
	else
		printf 'OK      %s\n' "$(basename "$file")"
	fi
done

printf '\n%s\n' '=== Resumo ==='
printf 'Validos:   %d | falhas: %d\n' "$valid_total" "$valid_failures"
printf 'Invalidos: %d | aceitos indevidamente: %d\n' \
	"$invalid_total" "$invalid_failures"

if [ "$valid_failures" -ne 0 ] || [ "$invalid_failures" -ne 0 ]; then
	exit 1
fi

exit 0
