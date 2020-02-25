void display_table(Symbol *table, int n)
{

    int i;

    printf("%-20s %-10s %-20s %-10s\n",
           "SYMBOL", "TYPE", "ADDRESS", "LINE NO.");
    for (i = 0; i < n; i++)
    {
        char *sym = table[i].sym;
        /*
        char *type;
        switch (table[i].type) {                                                    
            case NUMBER:
                type = "NUMBER";                                                    
                break;
            case STRING:
                type = "STRING";                                                    
                break;                                                              
        }
        printf("%-20s %-10s %-20s %-10s\n",                                         
        sym, type, ad);                                                             
        */
        printf("%-20s %-10s %-20s %010d\n",
               sym, "", "", table[i].lineno);
    }
}