import java.util.Scanner;

public class HW5 {

    public static void main(String[] args) {

        Scanner scanner = new Scanner(System.in);
        int n_row = scanner.nextInt();
        int n_col = scanner.nextInt();

        char[][] matrix = new char[n_row][n_col];
        for (int i = 0; i < n_row; i++) {
            for (int j = 0; j < n_col; j++) {
                String chr = scanner.next();
                matrix[i][j] = chr.charAt(0);
            }
        }

        String word = scanner.next();
        scanner.close();

        boolean[][] visited = new boolean[n_row][n_col];
        boolean found = false;
        char[] chrs = word.toCharArray();
        for (int i = 0; i < n_row; i++) {
            for (int j = 0; j < n_col; j++) {
                if (matrix[i][j] == chrs[0] && search(matrix, chrs, 0, i, j, n_row, n_col, visited)) {
                    found = true;
                    break;  
                }
            }
            if (found) {
                break;
            }
        }
        System.out.println(found);

    }

    public static boolean search(char[][] matrix, char[] word, int cur_index, int cur_row, int cur_col, int n_row, int n_col, boolean[][] visited) {
        if (cur_index == word.length) {
            return true;
        }
        if (cur_row < 0 || cur_row >= n_row || cur_col < 0 || cur_col >= n_col || visited[cur_row][cur_col] || matrix[cur_row][cur_col] != word[cur_index]) {
            return false;
        }
        visited[cur_row][cur_col] = true;
        if (search(matrix, word, cur_index + 1, cur_row + 1, cur_col, n_row, n_col, visited) ||
            search(matrix, word, cur_index + 1, cur_row - 1, cur_col, n_row, n_col, visited) ||
            search(matrix, word, cur_index + 1, cur_row, cur_col + 1, n_row, n_col, visited) ||
            search(matrix, word, cur_index + 1, cur_row, cur_col - 1, n_row, n_col, visited)) {
            return true;
        }
        visited[cur_row][cur_col] = false;
        return false;
    }
}