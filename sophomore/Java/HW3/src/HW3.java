import java.util.Scanner;

public class HW3 {

    public static void main(String[] args) {

        // read input
        int x, y;
        Point main_point = new Point();
        Point other_point = new Point();
        Scanner scanner = new Scanner(System.in);
        
        x = scanner.nextInt();
        y = scanner.nextInt();
        main_point.Set(x, y);
        x = scanner.nextInt();
        y = scanner.nextInt();
        other_point.Set(x, y);
        x = scanner.nextInt();
        y = scanner.nextInt();
        scanner.close();
        
        // main function
        main_point.PrintPosition();
        main_point.Move(x, y);
        main_point.PrintPosition();
        main_point.Rotate();
        main_point.PrintPosition();
        main_point.Rotate();
        main_point.PrintPosition();
        main_point.Rotate();
        main_point.PrintPosition();
        main_point.Rotate();
        main_point.PrintPosition();
        System.out.println(main_point.calculateManhattanDistance(other_point));
        System.out.println(main_point.ChebyshevDistance(other_point));
    }
}