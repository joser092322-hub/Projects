#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
    char nome[20];
    int hp;
    int hp_max;
    int ataque;
    int defesa;
} Pokemon;

void mostrar_status(Pokemon *pokemon) {
    printf("\n--- %s ---\n", pokemon->nome);
    printf("HP: %d/%d\n", pokemon->hp, pokemon->hp_max);
    printf("Ataque: %d\n", pokemon->ataque);
    printf("Defesa: %d\n", pokemon->defesa);
}

int calcular_dano(Pokemon *atacante, Pokemon *defensor) {
    int dano;

    dano = atacante->ataque - defensor->defesa / 2;

    dano += rand() % 6;

    if (dano < 1) {
        dano = 1;
    }

    return dano;
}

void atacar(Pokemon *atacante, Pokemon *defensor) {
    int dano = calcular_dano(atacante, defensor);

    defensor->hp -= dano;

    if (defensor->hp < 0) {
        defensor->hp = 0;
    }

    printf("\n%s atacou %s!\n", atacante->nome, defensor->nome);
    printf("Causou %d de dano!\n", dano);
}

void usar_pocao(Pokemon *pokemon, int *pocoes) {
    int cura = 20;

    if (*pocoes <= 0) {
        printf("\nVocê não tem mais poções!\n");
        return;
    }

    pokemon->hp += cura;

    if (pokemon->hp > pokemon->hp_max) {
        pokemon->hp = pokemon->hp_max;
    }

    (*pocoes)--;

    printf("\n%s recuperou HP!\n", pokemon->nome);
    printf("HP atual: %d/%d\n", pokemon->hp, pokemon->hp_max);
    printf("Poções restantes: %d\n", *pocoes);
}

int main() {
    Pokemon pokemons[3] = {
        {"Charmander", 100, 100, 25, 10},
        {"Squirtle", 110, 110, 20, 15},
        {"Bulbasaur", 105, 105, 22, 12}
    };

    Pokemon jogador;
    Pokemon inimigo;

    int escolha;
    int escolha_inimigo;
    int turno;
    int pocoes = 3;

    srand(time(NULL));

    printf("=================================\n");
    printf("       POKEMON BATTLE\n");
    printf("=================================\n");

    printf("\nEscolha seu Pokemon:\n");
    printf("1 - Charmander\n");
    printf("2 - Squirtle\n");
    printf("3 - Bulbasaur\n");

    printf("\nEscolha: ");
    scanf("%d", &escolha);

    if (escolha < 1 || escolha > 3) {
        printf("\nEscolha invalida!\n");
        return 1;
    }

    jogador = pokemons[escolha - 1];

    escolha_inimigo = rand() % 3;

    while (escolha_inimigo == escolha - 1) {
        escolha_inimigo = rand() % 3;
    }

    inimigo = pokemons[escolha_inimigo];

    printf("\nVocê escolheu %s!\n", jogador.nome);
    printf("O adversário escolheu %s!\n", inimigo.nome);

    turno = 1;

    while (jogador.hp > 0 && inimigo.hp > 0) {

        printf("\n=================================\n");
        printf("              TURNO %d\n", turno);
        printf("=================================\n");

        printf("\n%s: %d/%d HP\n",
               jogador.nome,
               jogador.hp,
               jogador.hp_max);

        printf("%s: %d/%d HP\n",
               inimigo.nome,
               inimigo.hp,
               inimigo.hp_max);

        printf("\n1 - Atacar\n");
        printf("2 - Usar pocao\n");
        printf("3 - Ver status\n");
        printf("Escolha: ");
        scanf("%d", &escolha);

        if (escolha == 1) {
            atacar(&jogador, &inimigo);
        }

        else if (escolha == 2) {
            usar_pocao(&jogador, &pocoes);
        }

        else if (escolha == 3) {
            mostrar_status(&jogador);
            mostrar_status(&inimigo);
            continue;
        }

        else {
            printf("\nOpcao invalida!\n");
            continue;
        }

        if (inimigo.hp <= 0) {
            break;
        }

        printf("\nTurno do adversario!\n");
        atacar(&inimigo, &jogador);

        turno++;
    }

    printf("\n=================================\n");

    if (jogador.hp > 0) {
        printf("         VOCE VENCEU!\n");
        printf("        %s venceu!\n", jogador.nome);
    } else {
        printf("         VOCE PERDEU!\n");
        printf("        %s venceu!\n", inimigo.nome);
    }

    printf("=================================\n");

    return 0;
}
