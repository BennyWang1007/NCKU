import java.util.ArrayList;
import java.util.Scanner;
import java.awt.Point;

public class Solution3 {
    public static void main(String[] args) {
        int[][] board = new int[8][8];
        int curX = 1, curY = 1;
        Scanner scanner = new Scanner(System.in);
        while(scanner.hasNextInt()) {
            for (int i = 0; i < 36; i ++) {
                board[curY][curX] = scanner.nextInt();
                curX++;
                if (curX == 7) {
                    curX = 1;
                    curY++;
                }
            }
            int n = scanner.nextInt();
            for (int i = 0; i < n; i ++) {
                int x1, y1, x2, y2;
                x1 = scanner.nextInt();
                y1 = scanner.nextInt();
                x2 = scanner.nextInt();
                y2 = scanner.nextInt();
                // System.out.println("(" + x1 + ", " + y1 + "), (" + x2 + ", " + y2 + ")");
                
                boolean reach = reachable(board, new Point(x1+1, y1+1), new Point(x2+1, y2+1));
                if (reach) {
                    System.out.println("pair matched");
                    board[y1+1][x1+1] = 0;
                    board[y2+1][x2+1] = 0;
                } else {
                    System.out.println("bad pair");
                }
                // print results
                // for(int[] row : board) {
                //     boolean notFirstCol = false;
                //     for(int num : row) {
                //         if (notFirstCol) System.out.print(" ");
                //         System.out.print(num);
                //         notFirstCol = true;
                //     }
                //     System.out.println("");
                // }
                // System.out.println("");
            }

        }
        scanner.close();

        // print results
        // for(int[] row : board) {
        //     boolean notFirstCol = false;
        //     for(int n : row) {
        //         if (notFirstCol) System.out.print(" ");
        //         System.out.print(n);
        //         notFirstCol = true;
        //     }
        //     System.out.println("");
        // }
    }

    public static boolean reachable(int[][] board, Point p1, Point p2) {

        if (p1.x == p2.x && Math.abs(p1.y - p2.y) == 1) return true;
        if (p1.y == p2.y && Math.abs(p1.x - p2.x) == 1) return true;


        ArrayList<Point> uds = goUpDown(board, p1);
        for (Point p : uds) {
            if (p.x == p2.x && Math.abs(p.y - p2.y) == 1) return true;
            if (p.y == p2.y && Math.abs(p.x - p2.x) == 1) return true;
        }

        // print
        // System.out.println("list first UD :");
        // System.out.println(uds);


        ArrayList<Point> oneTurn1D = new ArrayList<>();
        for(Point p : uds) {
            ArrayList<Point> tmp = goLeftRight(board, p);
            for (Point pReached : tmp) {
                oneTurn1D.add(pReached);
            }
        }

        for (Point p : oneTurn1D) {
            if (p.x == p2.x && Math.abs(p.y - p2.y) == 1) return true;
            if (p.y == p2.y && Math.abs(p.x - p2.x) == 1) return true;
        }

        // print
        // System.out.println("list one trun :");
        // System.out.println(oneTurn1D);

        ArrayList<Point> twoTurn1D = new ArrayList<>();
        for (Point p : oneTurn1D) {
            ArrayList<Point> tmp = goUpDown(board, p);
            for (Point pReached : tmp) {
                twoTurn1D.add(pReached);
            }
        }

        for (Point p : twoTurn1D) {
            if (p.x == p2.x && p.y == p2.y) return true;
        }

        // print
        // System.out.println("list two trun :");
        // System.out.println(twoTurn1D);

        // reach without turning
        ArrayList<Point> lrs = goLeftRight(board, p1);
        for (Point p : lrs) {
            if (p.x == p2.x && Math.abs(p.y - p2.y) == 1) return true;
            if (p.y == p2.y && Math.abs(p.x - p2.x) == 1) return true;
        }

        // print
        // System.out.println("list first LR :");
        // System.out.println(lrs);

        // reach with 1 turn
        oneTurn1D = new ArrayList<>();
        for(Point p : lrs) {
            ArrayList<Point> tmp = goUpDown(board, p);
            for (Point pReached : tmp) {
                oneTurn1D.add(pReached);
            }
        }

        for (Point p : oneTurn1D) {
            if (p.x == p2.x && Math.abs(p.y - p2.y) == 1) return true;
            if (p.y == p2.y && Math.abs(p.x - p2.x) == 1) return true;
        }

        // reach with 2 turns
        twoTurn1D = new ArrayList<>();
        for (Point p : oneTurn1D) {
            ArrayList<Point> tmp = goLeftRight(board, p);
            for (Point pReached : tmp) {
                twoTurn1D.add(pReached);
            }
        }

        for (Point p : twoTurn1D) {
            if (p.x == p2.x && p.y == p2.y) return true;
            if (p.y == p2.y && p2.y == p2.y) return true;
        }

        return false;
        
    }

    public static ArrayList<Point> goUpDown(int[][] board, Point p) {
        // System.out.println("Go Updown(" + p.x + ", " + p.y + ")");
        ArrayList<Point> list = new ArrayList<>();
        int x = p.x, y = p.y - 1;
        while(y >= 0) {
            // System.out.println(board[y][x]);
            if (board[y][x] == 0) list.add(new Point(x, y));
            else break;
            y--;
        }
        y = p.y + 1;
        while(y <= 7) {
            if (board[y][x] == 0) list.add(new Point(x, y));
            else break;
            y++;
        }
        return list;
    }

    public static ArrayList<Point> goLeftRight(int[][] board, Point p) {
        ArrayList<Point> list = new ArrayList<>();
        int x = p.x - 1, y = p.y;
        while(x >= 0) {
            if (board[y][x] == 0) list.add(new Point(x, y));
            else break;
            x--;
        }
        x = p.x + 1;
        while(x <= 7) {
            if (board[y][x] == 0) list.add(new Point(x, y));
            else break;
            x++;
        }
        return list;
    }
}
