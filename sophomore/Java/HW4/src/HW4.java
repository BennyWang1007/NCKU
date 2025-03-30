import java.util.Scanner;

public class HW4 {

    public static void main(String[] args) {

        Scanner scanner = new Scanner(System.in);
        int n_col = scanner.nextInt();
        // int MAX_EQ = 99999;
        double aug_temp[][] = new double[n_col][n_col+1];
        int idx = 0;

        // read augmented matrix
        while (true) {
            double a0 = scanner.nextDouble();
            if (a0 == -999) break;
            aug_temp[idx][0] = a0;
            for (int i = 1; i < n_col + 1; i++) {
                aug_temp[idx][i] = scanner.nextDouble();
            }
            idx++;
        }
        scanner.close();
        int n_row = idx;
        double matrix[][] = new double[n_row][n_col];
        for (int i = 0; i < n_row; i++) {
            for (int j = 0; j < n_col; j++) {
                matrix[i][j] = aug_temp[i][j];
            }
        }
        
        double aug_matrix[][] = new double[n_row][n_col + 1];
        for (int i = 0; i < n_row; i++) {
            for (int j = 0; j < n_col + 1; j++) {
                aug_matrix[i][j] = aug_temp[i][j];
            }
        }
        
        int rank = Rank(matrix, n_row, n_col);
        int rank_augmented = Rank(aug_matrix, n_row, n_col + 1);
        // System.out.println("Rank of matrix: " + rank + " Rank of augmented matrix: " + rank_augmented);
        if (rank < rank_augmented) {
            System.out.println("No solution");
        } else if (rank == rank_augmented) {
            if (rank == n_col) {
                System.out.println("The only solution");
            } else {
                System.out.println("Infinite solutions");
            }
        }
        else {
            System.out.println("Infinite solutions");
        }
    }


    /**
     * Find the rank of a matrix
     * @param arr 2D array
     * @param n_row number of rows
     * @param n_col number of columns
     * @return rank of the matrix
     */
    public static int Rank(double arr[][], int n_row, int n_col) {
        
        // elimination
        int row = 0;
        for (int col = 0; col < n_col; col++) {

            // skip if all elements in the column are zero
            if (row == n_row) {
                break;
            }

            // find the first non-zero row
            int first_non_zero_row_ = findFirstNonZeroRow(arr, row, col, n_row);
            // System.out.println("col: " + col + "start row: " + row + " first_non_zero_row: " + first_non_zero_row_);
            if (first_non_zero_row_ == n_row) continue;
            else swapRow(arr, row, first_non_zero_row_, n_col);

            // subtract the row from other rows
            for (int i = 0; i < n_row; i++) {
                if (i != row && arr[i][col] != 0) {
                    subtractRow(arr, i, row, col, n_col);
                }
            }
            row++;
        }

        // rank = number of non-zero rows
        int rank = 0;
        for (int i = 0; i < n_row; i++) {
            if (!isZeroRow(arr[i], n_col)) {
                rank++;
            }
        }
        // printMatrix(arr, n_row, n_col, "Reduced matrix:");
        // System.out.println("Rank: " + rank);
        
        return rank;
    }

    public static void printMatrix(double arr[][], int n_row, int n_col, String msg) {
        System.out.println(msg);
        for (int i = 0; i < n_row; i++) {
            for (int j = 0; j < n_col; j++) {
                System.out.print(arr[i][j] + " ");
            }
            System.out.println();
        }
        System.out.println();
    }


    public static void swapRow(double arr[][], int row1, int row2, int n_col) {
        for (int i = 0; i < n_col; i++) {
            double temp = arr[row1][i];
            arr[row1][i] = arr[row2][i];
            arr[row2][i] = temp;
        }
    }
    
    /**
     * Subtract row2 from row1 as many times as possible
     * @param arr 2D array
     * @param row1 row to be subtracted
     * @param row2 row to be subtracted from
     * @param col target column
     * @param n_col number of columns
     */
    public static void subtractRow(double arr[][], int row1, int row2, int col, int n_col) {
        if (arr[row1][col] == 0 || arr[row2][col] == 0) {
            return;
        }
        double multiple = arr[row1][col] / arr[row2][col];
        for (int i = 0; i < n_col; i++) {
            arr[row1][i] -= multiple * arr[row2][i];
        }
    }


    public static void inverseSignRow(double arr[][], int row, int n_col) {
        for (int i = 0; i < n_col; i++) {
            arr[row][i] = -arr[row][i];
        }
    }


    public static int findFirstNonZeroRow(double arr[][], int start_row, int col, int n_row) {
        int first_non_zero_row = start_row;
        double tolerance = 1e-6;
        while (first_non_zero_row < n_row && Math.abs(arr[first_non_zero_row][col]) < tolerance) {
            first_non_zero_row++;
        }
        return first_non_zero_row;
    }


    public static boolean isZeroRow(double arr[], int n_col) {
        double tolerance = 1e-6;
        for (int i = 0; i < n_col; i++) {
            if (Math.abs(arr[i]) > tolerance) {
                return false;
            }
        }
        return true;
    }

}