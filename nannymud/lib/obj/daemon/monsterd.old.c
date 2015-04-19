/*------------------------------------------------------------*/

int standard_hp(int lvl)
{
#if 1
  switch(lvl)
  {
  case 0:
  case 1: return 50;
  case 2: return 60;
  case 3: return 70;
  case 4: return 80;
  case 5: return 90;
  case 6: return 100;
  case 7: return 110;
  case 8: return 120;
  case 9: return 130;
  case 10: return 140;
  case 11: return 150;
  case 12: return 160;
  case 13: return 170;
  case 14: return 180;
  case 15: return 200;
  case 16: return 225;
  case 17: return 250;
  case 18: return 300;
  case 19: return 350;
  case 20: return 400;
  default:
    return 400 + 50*(lvl-20 + ((lvl-20)*lvl)/20);
  }
#else
  if (lvl<21)
    return ({50, 60, 70, 80, 90, 
	       100, 110, 120, 130, 140, 
	       150, 160, 170, 180, 200, 
	       225, 250, 300, 350, 400
	     })[lvl - 1];
  else
    return 400 + 50*(lvl-20 + ((lvl-20)*lvl)/20);
#endif
} /* standard_hp */
/*------------------------------------------------------------*/

int standard_wc(int lvl)
{
#if 1
  switch(lvl)
  {
  case 0:
  case 1:
  case 2:
  case 3: return 3;
  case 4:
  case 5: return 4;
  case 6: return 5;
  case 7: return 6;
  case 8: return 7;
  case 9: return 8;
  case 10: return 9;
  case 11: return 10;
  case 12: return 11;
  case 13: return 12;
  case 14: return 13;
  case 15: return 15;
  case 16: return 16;
  case 17: return 18;
  case 18: return 19;
  case 19: return 20;
  case 20: return 25;
  default:
    return 25 + 5*(lvl-20);
  }
#else
    
  if (lvl<21) 
    return ({3, 3, 3, 4, 4, 
	     5, 6, 7, 8, 9, 
	     10, 11, 12, 13, 15, 
	     16, 18, 19, 20, 25
	   })[lvl - 1];
  else
    return 25 + 5*(lvl-20);
#endif
} /* standard_wc */
/*------------------------------------------------------------*/

int standard_ac(int lvl)
{
#if 0
  switch(lvl)
  {
  case 0:
  case 1:
  case 2: return 0;
  case 3:
  case 4: return 1;
  case 5:
  case 6: return 2;
  case 7: 
  case 8: 
  case 9: return 3;
  case 10:
  case 11:
  case 12: return 4;
  case 13:
  case 14: return 5;
  case 15: return 6;
  case 16: return 7;
  case 17:
  case 18: return 8;
  case 19: return 9;
  case 20: return 10;
  default:
    return 10 + 2*(lvl-20);
  }
#else
  if (lvl<21)
    return ({0, 0, 1, 1, 2,
	     2, 3, 3, 3, 4, 
	     4, 4, 5, 5, 6, 
	     7, 8, 8, 9, 10
	     })[lvl - 1];
  else
    return 10 + 2*(lvl-20);
#endif
}				/* standard_ac */
/*------------------------------------------------------------*/
