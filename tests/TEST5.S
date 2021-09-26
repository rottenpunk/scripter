main(argc, argv)
{

   Input = receive(Session);

   do {
      Input = receive(Session);
      var1 = var2 + var3 / --var4 + (var5 + (var6 + (var7 * var8)));
   } while (strncmp(Input, "Input", 5) == 0);

   do
      send(Session, "Test1");
   while (Input != 5);

   return var2;
}

send(var1, var2)
{
   var1 += var2;
}

