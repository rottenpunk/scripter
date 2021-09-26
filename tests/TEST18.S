//----------------------------------------------------------------------------
//  test18.s - test function calls with local variables...
//----------------------------------------------------------------------------


main()
{
   Total = 0;
   Count = 0;

   for (i = 0; i < 100000 ; i++ ) {
      Func1(i, i + 1);
   }


   printf("Total = %d\n", Total);

}


Func1 (Var1, Var2)
{
   //auto SubTotal;

   Count++;
   SubTotal = Var1 + Var2;
   Total += Var1 + Var2;

   //printf("%d -- SubTotal = %d, Total = %d\n", Count, SubTotal, Total);

}

