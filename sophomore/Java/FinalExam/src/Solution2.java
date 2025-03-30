import java.util.Scanner;

public class Solution2 {
    public static void main(String[] args) {
        int[][] board = new int[10][10];
        int curX = 0, curY = 0;
        Scanner scanner = new Scanner(System.in);
        while(scanner.hasNextInt()) {
            board[curY][curX] = scanner.nextInt();
            curX++;
            if (curX == 10) {
                curX = 0;
                curY++;
            }
        }
        scanner.close();
        for(int y = 0; y < 10; y ++) {
            for (int x = 0; x < 10; x++) {
                testLineUp(board, x, y);
            }
        }

        // print results
        for(int[] row : board) {
            boolean notFirstCol = false;
            for(int n : row) {
                if (notFirstCol) System.out.print(" ");
                if (n == 2)System.out.print("X");
                else System.out.print(n);
                notFirstCol = true;
            }
            System.out.println("");
        }

    }

    public static void testLineUp(int[][] board, int x, int y) {
        if (board[y][x] != 0) return;
        if (x >= 4) {
            if (board[y][x-4] == 1 && board[y][x-3] == 1 && board[y][x-2] == 1 && board[y][x-1] == 1) {
                board[y][x] = 2;
            }
        }
        if (x >= 3 && x <= 8) {
            if (board[y][x+1] == 1 && board[y][x-3] == 1 && board[y][x-2] == 1 && board[y][x-1] == 1) {
                board[y][x] = 2;
            }
        }
        if (x >= 2 && x <= 7) {
            if (board[y][x+1] == 1 && board[y][x+2] == 1 && board[y][x-2] == 1 && board[y][x-1] == 1) {
                board[y][x] = 2;
            }
        }
        if (x >= 1 && x <= 6) {
            if (board[y][x+1] == 1 && board[y][x+2] == 1 && board[y][x+3] == 1 && board[y][x-1] == 1) {
                board[y][x] = 2;
            }
        }
        if (x <= 5) {
            if (board[y][x+1] == 1 && board[y][x+2] == 1 && board[y][x+3] == 1 && board[y][x+4] == 1) {
                board[y][x] = 2;
            }
        }
        
        if (y >= 4) {
            if (board[y-4][x] == 1 && board[y-3][x] == 1 && board[y-2][x] == 1 && board[y-1][x] == 1) {
                board[y][x] = 2;
            }
        }
        if (y >= 3 && y <= 8) {
            if (board[y+1][x] == 1 && board[y-3][x] == 1 && board[y-2][x] == 1 && board[y-1][x] == 1) {
                board[y][x] = 2;
            }
        }
        if (y >= 2 && y <= 7) {
            if (board[y+1][x] == 1 && board[y+2][x] == 1 && board[y-2][x] == 1 && board[y-1][x] == 1) {
                board[y][x] = 2;
            }
        }
        if (y >= 1 && y <= 6) {
            if (board[y+1][x] == 1 && board[y+2][x] == 1 && board[y+3][x] == 1 && board[y-1][x] == 1) {
                board[y][x] = 2;
            }
        }
        if (y <= 5) {
            if (board[y+1][x] == 1 && board[y+2][x] == 1 && board[y+3][x] == 1 && board[y+4][x] == 1) {
                board[y][x] = 2;
            }
        }

        if (x >= 4 && y >= 4) {
            if (board[y-4][x-4] == 1 && board[y-3][x-3] == 1 && board[y-2][x-2] == 1 && board[y-1][x-1] == 1) {
                board[y][x] = 2;
            }
        }
        if (x >= 3 && y >= 3 && x <= 8 && y <= 8) {
            if (board[y+1][x+1] == 1 && board[y-3][x-3] == 1 && board[y-2][x-2] == 1 && board[y-1][x-1] == 1) {
                board[y][x] = 2;
            }
        }
        if (x >= 2 && y >= 2 && x <= 7 && y <= 7) {
            if (board[y+1][x+1] == 1 && board[y+2][x+2] == 1 && board[y-2][x-2] == 1 && board[y-1][x-1] == 1) {
                board[y][x] = 2;
            }
        }
        if (x >= 1 && y >= 1 && x <= 6 && y <= 6) {
            if (board[y+1][x+1] == 1 && board[y+2][x+2] == 1 && board[y+3][x+3] == 1 && board[y-1][x-1] == 1) {
                board[y][x] = 2;
            }
        }
        if (x <= 5 && y <= 5) {
            if (board[y+1][x+1] == 1 && board[y+2][x+2] == 1 && board[y+3][x+3] == 1 && board[y+4][x+4] == 1) {
                board[y][x] = 2;
            }
        }


        if (x >= 4 && y <= 5) {
            if (board[y+4][x-4] == 1 && board[y+3][x-3] == 1 && board[y+2][x-2] == 1 && board[y+1][x-1] == 1) {
                board[y][x] = 2;
            }
        }
        if (x >= 3 && y <= 6 && x <= 8 && y >= 1) {
            if (board[y-1][x+1] == 1 && board[y+3][x-3] == 1 && board[y+2][x-2] == 1 && board[y+1][x-1] == 1) {
                board[y][x] = 2;
            }
        }
        if (x >= 2 && y <= 7 && x <= 7 && y >= 2) {
            if (board[y-1][x+1] == 1 && board[y-2][x+2] == 1 && board[y+2][x-2] == 1 && board[y+1][x-1] == 1) {
                board[y][x] = 2;
            }
        }
        if (x >= 1 && y <= 8 && x <= 6 && y >= 3) {
            if (board[y-1][x+1] == 1 && board[y-2][x+2] == 1 && board[y-3][x+3] == 1 && board[y+1][x-1] == 1) {
                board[y][x] = 2;
            }
        }
        if (x <= 5 && y >= 4) {
            if (board[y-1][x+1] == 1 && board[y-2][x+2] == 1 && board[y-3][x+3] == 1 && board[y-4][x+4] == 1) {
                board[y][x] = 2;
            }
        }
    }
}
