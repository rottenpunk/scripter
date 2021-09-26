//----------------------------------------------------------------------------
//  test19.s - This test is same as test18.s except no local vars...
//----------------------------------------------------------------------------


main()
{
   Total = 0;
   Count = 0;

   for (i = 0; i < 1000000 ; i++ ) {
      //Func1(i, i + 1);
      Total += i + i + 1;
      Count++;
      if (Count % 10000 == 0)
         printf("%d -- Total = %d\n", Count, Total);
   }


   printf("Total = %d\n", Total);

}


Func1 (Var1, Var2)
{
   //auto SubTotal;

   Count++;

   SubTotal = Var1 + Var2;
   Total += Var1 + Var2;

   if (Count % 10000 == 0)
      printf("%d -- SubTotal = %d, Total = %d\n", Count, SubTotal, Total);

}

