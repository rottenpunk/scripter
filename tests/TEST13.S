main ()
{
   for (i = 0; i < 10; i += 1)  {
      if (i < 3)
         break;
      if (i > 3)
         continue;
      send("here");
   }

}
