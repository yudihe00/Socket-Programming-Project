#include <stdio.h>
  #define SIZE 5
  void change_array(double x[], int s);


   int main()
   {
        double x[SIZE] = {3, 5, 6, 12, 32};
        printf("The array is as: \n");
        for(int i = 0;i<SIZE;i++)
            printf("\n%f",x[i]);

        //fill_array(x, SIZE);

        //print_array(x, SIZE);
        change_array(x, SIZE);
        printf("After change, the array is: \n");
        for(int i = 0;i<SIZE;i++)
            printf("\n%f",x[i]);
        // print_array(x, SIZE);
        return 0;
    }


    void change_array(double x[], int s)
    {
        int i=0;

        for (i=0; i<s; i++) 
        {
            if (x[i] < 10) 
            {
                 (x[i] = (x[i] * 2));
            }
        }
    }