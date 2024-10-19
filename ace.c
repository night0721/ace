#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <termios.h>

#define CLEAR_SCREEN() printf("\033[H\033[J")
#define MOVE_CURSOR(x, y) printf("\033[%d;%dH", (x), (y))
#define RESET_COLOR() printf("\033[0m")

typedef struct {
	int value; /* 1 to 13 (Ace to King) */
	int suit; /* 0 to 3 (Hearts, Spades, Diamonds, Clubs) */
} card;

card deal_card();
int calculate_score(card hand[], int num_cards);
void display_hand(card hand[], int num_cards, int is_player, int total);

struct termios orig_termios;

int main()
{
	srand(time(NULL));

	card player_hand[10], cpu_hand[10];
	int player_num_cards = 0, cpu_num_cards = 0;

	/* Set terminal to return read syscall without enter */
	struct termios new_termios;

	tcgetattr(STDIN_FILENO, &orig_termios);
	new_termios = orig_termios;

	new_termios.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);


	for (int i = 0; i < 2; i++) {
		player_hand[player_num_cards++] = deal_card();
		cpu_hand[cpu_num_cards++] = deal_card();
	}

	int player_score;
	int cpu_score;

	while (1) {
		CLEAR_SCREEN();
		MOVE_CURSOR(1, 1);
		printf("You			   Computer\n");
		

		player_score = calculate_score(player_hand, player_num_cards);
		cpu_score = calculate_score(cpu_hand, cpu_num_cards);

		display_hand(player_hand, player_num_cards, 1, player_score);
		display_hand(cpu_hand, cpu_num_cards, 0, cpu_score);

	    /* Check if the player has busted */
	    if (player_score > 21) {
	        printf("\n\033[38;2;255;0;0mBust! You lose.\033[0m\n");
	        break;
	    }

	    printf("\nHit (h) or stand (s)? ");
		fflush(stdout);

		char action;
		read(STDIN_FILENO, &action, 1);

	    if (action == 'h') {
	        player_hand[player_num_cards++] = deal_card();
	    } else if (action == 's') {
	        break;
	    }
	}

	/* Computer's turn to deal
	 * Customise here if you want more wins
	 */
	while (cpu_score < 17) {
	    cpu_hand[cpu_num_cards++] = deal_card();
		cpu_score = calculate_score(cpu_hand, cpu_num_cards);
	}

	CLEAR_SCREEN();
	MOVE_CURSOR(1, 1);

	player_score = calculate_score(player_hand, player_num_cards);
	cpu_score = calculate_score(cpu_hand, cpu_num_cards);
	
	/* Determine win or lose */
	if (player_score > 21) {
	    printf("\033[38;2;255;0;0mComputer wins!\033[0m\n");
	} else if (cpu_score > 21) {
	    printf("\033[38;2;0;255;0mYou win!\033[0m\n");
	} else if (player_score > cpu_score) {
	    printf("\033[38;2;0;255;0mYou win!\033[0m\n");
	} else if (player_score < cpu_score) {
	    printf("\033[38;2;255;0;0mComputer wins!\033[0m\n");
	} else {
	    printf("\033[38;2;255;255;0mDraw!\033[0m\n");
	}

	display_hand(player_hand, player_num_cards, 1, player_score);
	display_hand(cpu_hand, cpu_num_cards, 0, cpu_score);

	printf("\n");
	/* Reset terminal */
	tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);
	return 0;
}

card deal_card()
{
	card card;
	card.value = rand() % 13 + 1;
	card.suit = rand() % 4;
	return card;
}

int calculate_score(card hand[], int num_cards)
{
	int score = 0, num_aces = 0;

	for (int i = 0; i < num_cards; i++) {
	    if (hand[i].value == 1) {
	        score += 11;
	        num_aces++;
	    } else if (hand[i].value > 10) {
	        score += 10;
	    } else {
	        score += hand[i].value;
	    }
	}

	while (score > 21 && num_aces > 0) {
	    score -= 10;
	    num_aces--;
	}

	return score;
}

void display_hand(card hand[], int num_cards, int is_player, int total)
{
	const char *suits[] = {"\033[38;2;255;0;0m♥\033[0m", "\033[38;2;0;0;255m♠\033[0m", "\033[38;2;255;0;255m♦\033[0m", "\033[38;2;0;255;0m♣\033[0m"};
	
	if (is_player) {
	    printf("Cards - ");
	} else {
		/* Alignment */
	    MOVE_CURSOR(2, 28);
	    printf("Cards - ");
	}

	for (int i = 0; i < num_cards; i++) {
		/* Bold */
		printf("\033[1m");
	    switch (hand[i].value) {
	        case 1:
	            printf("A%s ", suits[hand[i].suit]);
	            break;
	        case 11:
	            printf("J%s ", suits[hand[i].suit]);
	            break;
	        case 12:
	            printf("Q%s ", suits[hand[i].suit]);
	            break;
	        case 13:
	            printf("K%s ", suits[hand[i].suit]);
	            break;
	        default:
	            printf("%d%s ", hand[i].value, suits[hand[i].suit]);
	    }
	    RESET_COLOR();
	}

	printf("\n");
	if (is_player) {
	    printf("Total - ");
	} else {
	    MOVE_CURSOR(3, 28);
	    printf("Total - ");
	}

	/* Bold and inverted colors */
	printf("\033[1m\033[47;30m"); 
	printf(" %d ", total);
	RESET_COLOR(); 

	printf("\n");
}
