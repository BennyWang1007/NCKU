import java.util.Scanner;

public class HW6 {

    public static void main(String[] args) {

        Scanner scanner = new Scanner(System.in);
        String intput = scanner.next();
        // System.out.println(intput);
        scanner.close();

        boolean[] board_O = new boolean[9];
        boolean[] board_X = new boolean[9];

        int move_X = 0;
        int move_O = 0;

        for (int i = 0; i < 9; i++) {
            if (intput.charAt(i) == '#') {
                board_O[i] = false;
                board_X[i] = false;
                continue;
            }
            if (intput.charAt(i) == 'X') {
                board_O[i] = false;
                board_X[i] = true;
                move_X++;
                continue;
            } 
            if (intput.charAt(i) == 'O') {
                board_O[i] = true;
                board_X[i] = false;
                move_O++;
                continue;
            }
        }
        int count_X = find_match_count(board_X);
        int count_O = find_match_count(board_O);

        if (count_X > 5) {
            System.out.println("invalid");
            return;
        }
        if ((count_X == 1 || count_X == 2) && count_O == 0 && move_X == move_O + 1) {
            System.out.println("valid");
            return;
        }
        if (count_X == 0 && count_O == 1 && move_X == move_O) {
            System.out.println("valid");
            return;
        }
        if (count_X == 0 && count_O == 0 && ((move_X == move_O) || (move_X == move_O + 1))) {
            System.out.println("valid");
            return;
        }
        System.out.println("invalid");
    }

    public static int find_match_count(boolean board[]) {
        int count = 0;
        if (board[0] && board[1] && board[2]) count++;
        if (board[3] && board[4] && board[5]) count++;
        if (board[6] && board[7] && board[8]) count++;
        if (board[0] && board[3] && board[6]) count++;
        if (board[1] && board[4] && board[7]) count++;
        if (board[2] && board[5] && board[8]) count++;
        if (board[0] && board[4] && board[8]) count++;
        if (board[2] && board[4] && board[6]) count++;
        return count;
    }

    public static boolean is_cross_board(boolean board_X[], boolean board_O[]) {
        return board_X[0] && board_X[2] && board_X[4] && board_X[6] && board_X[8] && board_O[1] && board_O[3] && board_O[5] && board_O[7];
    }
}