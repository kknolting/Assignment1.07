#include <cstdio>
#include <string.h>
#include <sys/time.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <sstream>

#include "dungeon.h"
#include "pc.h"
#include "npc.h"
#include "move.h"
#include "utils.h"
#include "io.h"

using namespace std;

const char *victory =
  "\n                                       o\n"
  "                                      $\"\"$o\n"
  "                                     $\"  $$\n"
  "                                      $$$$\n"
  "                                      o \"$o\n"
  "                                     o\"  \"$\n"
  "                oo\"$$$\"  oo$\"$ooo   o$    \"$    ooo\"$oo  $$$\"o\n"
  "   o o o o    oo\"  o\"      \"o    $$o$\"     o o$\"\"  o$      \"$  "
  "\"oo   o o o o\n"
  "   \"$o   \"\"$$$\"   $$         $      \"   o   \"\"    o\"         $"
  "   \"o$$\"    o$$\n"
  "     \"\"o       o  $          $\"       $$$$$       o          $  ooo"
  "     o\"\"\n"
  "        \"o   $$$$o $o       o$        $$$$$\"       $o        \" $$$$"
  "   o\"\n"
  "         \"\"o $$$$o  oo o  o$\"         $$$$$\"        \"o o o o\"  "
  "\"$$$  $\n"
  "           \"\" \"$\"     \"\"\"\"\"            \"\"$\"            \""
  "\"\"      \"\"\" \"\n"
  "            \"oooooooooooooooooooooooooooooooooooooooooooooooooooooo$\n"
  "             \"$$$$\"$$$$\" $$$$$$$\"$$$$$$ \" \"$$$$$\"$$$$$$\"  $$$\""
  "\"$$$$\n"
  "              $$$oo$$$$   $$$$$$o$$$$$$o\" $$$$$$$$$$$$$$ o$$$$o$$$\"\n"
  "              $\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\""
  "\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"$\n"
  "              $\"                                                 \"$\n"
  "              $\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\""
  "$\"$\"$\"$\"$\"$\"$\"$\n"
  "                                   You win!\n\n";

const char *tombstone =
  "\n\n\n\n                /\"\"\"\"\"/\"\"\"\"\"\"\".\n"
  "               /     /         \\             __\n"
  "              /     /           \\            ||\n"
  "             /____ /   Rest in   \\           ||\n"
  "            |     |    Pieces     |          ||\n"
  "            |     |               |          ||\n"
  "            |     |   A. Luser    |          ||\n"
  "            |     |               |          ||\n"
  "            |     |     * *   * * |         _||_\n"
  "            |     |     *\\/* *\\/* |        | TT |\n"
  "            |     |     *_\\_  /   ...\"\"\"\"\"\"| |"
  "| |.\"\"....\"\"\"\"\"\"\"\".\"\"\n"
  "            |     |         \\/..\"\"\"\"\"...\"\"\""
  "\\ || /.\"\"\".......\"\"\"\"...\n"
  "            |     |....\"\"\"\"\"\"\"........\"\"\"\"\""
  "\"^^^^\".......\"\"\"\"\"\"\"\"..\"\n"
  "            |......\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"......"
  "..\"\"\"\"\"....\"\"\"\"\"..\"\"...\"\"\".\n\n"
  "            You're dead.  Better luck in the next life.\n\n\n";

void usage(char *name)
{
  fprintf(stderr,
          "Usage: %s [-r|--rand <seed>] [-l|--load [<file>]]\n"
          "          [-s|--save [<file>]] [-i|--image <pgm file>]\n"
          "          [-n|--nummon <count>]\n",
          name);

  exit(-1);
}

int main(int argc, char *argv[])
{
  dungeon_t d;
  time_t seed;
  class timeval tv;
  int i;
  uint32_t do_load, do_save, do_seed, do_image, do_save_seed, do_save_image;
  uint32_t long_arg;
  char *save_file;
  char *load_file;
  char *pgm_file;
  
  /* Quiet a false positive from valgrind. */
  memset(&d, 0, sizeof (d));

  //Parse and print data from monster file
  ifstream monsterFile;
  string path;

  string home = getenv("HOME");
  string game_dir = ".rlg327";
  string fileName = "monster_desc.txt";
  //sprintf(path, "%s/%s/%s", home, game_dir, fileName);

  path = home + '/' +  game_dir + '/' + fileName;
  monsterFile.open(path);

  string line;

  int firstIter = 1;
  int MonsterCount = 0;

  // cout << path << '\n';
  // cout << monsterFile.is_open() <<'\n';
  if(monsterFile.is_open()){

    while (getline(monsterFile, line)){

      //cout << line << '\n';
      //cout << line.compare("RLG327 MONSTER DESCRIPTION 1") << '\n';

      if(line.compare("RLG327 MONSTER DESCRIPTION 1") != 0 && (firstIter == 1)){

	return 0;
      }

      else{

	if(line.compare("BEGIN MONSTER") == 0){

	  MonsterCount++;
	}

      }

      firstIter = 0;

    }

    cout << MonsterCount << '\n';
    monsterFile.close();
    //return 0;

  }

  else{

    cout << "unable to open file.\n";
    return 0;

  }


  monster monsterArray[MonsterCount];
  //int startAdding = 0;
  int currMon = -1;
  string name;
  ifstream monsterFile_2;
  int foundDESC = 0;
  string description;
  string color;
  
  monsterFile_2.open(path);
    if(monsterFile_2.is_open()){
      cout <<"Opened File 2\n";

    while (getline(monsterFile_2, line)){

      // cout << line << '\n';

      if(line.compare("BEGIN MONSTER") == 0){

	currMon++;

	//cout << "Monster: "<< currMon << '\n';
      }

      

      //if name is in the line
      if(line.find("NAME") != string::npos){
	//cout << "Found name\n";
	name = "";

	for(int i = 5; i < (int)line.length(); i++){

	  name = name + line[i];
	}

	monsterArray[currMon].name = name;
	//cout << monsterArray[currMon].name << '\n';
      }

      else if(line.find("DESC") != string::npos){

	//cout <<"Found DESC\n";
	foundDESC = 1;

      }

      else if(foundDESC == 1){
	if(line.compare(".") == 0){
	  foundDESC = 0;
	  monsterArray[currMon].description = description;
	  description = "";

	  //cout << monsterArray[currMon].description;
	}

	else{
	  /*
	  string temp = "";

	  for(int i = 0; i < (int)line.length(); i++){

	    if(i == 77){

	      temp = temp +  line[i] + '\n';
	    }
	    else{

	      temp += line[i];
	    }

	  } */
	  description = description + line + '\n';

	  monsterArray[currMon].description = description;
	}
	//cout << line << '\n';
	//cout << foundDESC << '\n';
      }

      else if(line.find("COLOR") == 0){
	color = "";

	for(int i = 6; i < (int)line.length(); i++){

	  color = color + line[i];
	}

	if(color.compare("RED") == 0){
	  monsterArray[currMon].color = "RED";
	}

	else if(color.compare("GREEN") == 0){

	  monsterArray[currMon].color = "GREEN";
	}

	else if(color.compare("YELLOW") == 0){

	  monsterArray[currMon].color = "YELLOW";

	}

	else if(color.compare("BLUE") == 0){

	  monsterArray[currMon].color = "BLUE";

	}

	else if(color.compare("MAGENTA") == 0){

	  monsterArray[currMon].color = "MAGENTA";
	 
	}

	else if(color.compare("CYAN") == 0){

	  monsterArray[currMon].color = "CYAN";

	}

	else if(color.compare("WHITE") == 0){

	  monsterArray[currMon].color = "WHITE";
	}

	//cout << monsterArray[currMon].color << '\n';
      }

      else if(line.find("SPEED") != string::npos){
	string Sbase = "";
	string Sdice = "";
	string Ssides = "";
	int doDice = 0;
	int doSides = 0;

	//cout <<line[6] << '\n';

	//int base = line[6] - '0';

	//cout <<base << '\n';

	//monsterArray[currMon].speed.base = line[6] - '0';
       

	for(int i = 6; i < (int)line.length(); i++){

	  if(line[i] == '+'){
	    doDice = 1;
	  }

	  else if (line[i] == 'd'){

	    doSides = 1;
	    doDice = 0;
	  }

	  else if (doDice == 1){

	    Sdice = Sdice + line[i];
	  }

	  else if (doSides == 1){
	    Ssides = Ssides + line[i];
	  }

	  else{

	    Sbase = Sbase + line[i];
	  }
	}
	stringstream baseStr(Sbase);
	int base = 0;
	baseStr >> base;

	stringstream diceStr(Sdice);
	int dice = 0;
	diceStr >> dice;

	stringstream sidesStr(Ssides);
	int sides = 0;
	sidesStr >> sides;

	monsterArray[currMon].speed.base = base;
	monsterArray[currMon].speed.dice = dice;
	monsterArray[currMon].speed.sides = sides;

	//cout << monsterArray[currMon].speed.base << '+' << monsterArray[currMon].speed.dice << 'd' << monsterArray[currMon].speed.sides << '\n';
      }

      else if(line.find("ABIL") != string::npos){
	string tempAbil = "";

	for(int i = 5; i < (int)line.length(); i++){

	  tempAbil += line[i];

	}

	monsterArray[currMon].ability = tempAbil;

	//cout << monsterArray[currMon].ability << '\n';
      }

      else if(line.find("HP") != string::npos){
	string Sbase = "";
	string Sdice = "";
	string Ssides = "";
	int doDice = 0;
	int doSides = 0;

	for(int i = 3; i < (int)line.length(); i++){

	  if(line[i] == '+'){
	    doDice = 1;
	  }

	  else if (line[i] == 'd'){

	    doSides = 1;
	    doDice = 0;
	  }

	  else if (doDice == 1){

	    Sdice = Sdice + line[i];
	  }

	  else if (doSides == 1){
	    Ssides = Ssides + line[i];
	  }

	  else{

	    Sbase = Sbase + line[i];
	  }
	}
	stringstream baseStr(Sbase);
	int base = 0;
	baseStr >> base;

	stringstream diceStr(Sdice);
	int dice = 0;
	diceStr >> dice;

	stringstream sidesStr(Ssides);
	int sides = 0;
	sidesStr >> sides;

	monsterArray[currMon].HP.base = base;
	monsterArray[currMon].HP.dice = dice;
	monsterArray[currMon].HP.sides = sides;
	//cout << monsterArray[currMon].HP.base << '+' << monsterArray[currMon].HP.dice << 'd' << monsterArray[currMon].HP.sides << '\n';
      }

      else if(line.find("DAM") != string::npos){
	string Sbase = "";
	string Sdice = "";
	string Ssides = "";
	int doDice = 0;
	int doSides = 0;

	for(int i = 4; i < (int)line.length(); i++){

	  if(line[i] == '+'){
	    doDice = 1;
	  }

	  else if (line[i] == 'd'){

	    doSides = 1;
	    doDice = 0;
	  }

	  else if (doDice == 1){

	    Sdice = Sdice + line[i];
	  }

	  else if (doSides == 1){
	    Ssides = Ssides + line[i];
	  }

	  else{

	    Sbase = Sbase + line[i];
	  }
	}
	stringstream baseStr(Sbase);
	int base = 0;
	baseStr >> base;

	stringstream diceStr(Sdice);
	int dice = 0;
	diceStr >> dice;

	stringstream sidesStr(Ssides);
	int sides = 0;
	sidesStr >> sides;

	monsterArray[currMon].AD.base = base;
	monsterArray[currMon].AD.dice = dice;
	monsterArray[currMon].AD.sides = sides;
	//cout << monsterArray[currMon].AD.base << '+' << monsterArray[currMon].AD.dice << 'd' << monsterArray[currMon].AD.sides << '\n';
      }

      else if(line.find("SYMB") != string::npos){

	monsterArray[currMon].symbol = line[5];

	//cout << monsterArray[currMon].symbol << '\n';
      }

      else if(line.find("RRTY") != string::npos){

	string Srarity = "";

	for(int i = 5; i < (int)line.length(); i++){

	  Srarity += line[i];

	}

	stringstream rareS(Srarity);
	int rarity = 0;
	rareS >> rarity;

	monsterArray[currMon].rarity = rarity;

	cout <<monsterArray[currMon].rarity << '\n';
	
      }

    }
    monsterFile_2.close();
    cout << "File Closed\n";
   


 }

  else{
  

    cout << "unable to open file.\n";
    return 0;

  }

    //print monster array

    for(int i = 0; i < (int)sizeof(monsterArray); i++){

      //print name
      cout << monsterArray[i].name << '\n';

      //print desc
      cout << monsterArray[i].description << '\n';

      //print symbol
      cout << monsterArray[i].symbol << '\n';

      //print color
      cout << monsterArray[i].color << '\n';

      //print speed
      cout << monsterArray[i].speed.base << '+' << monsterArray[i].speed.dice << 'd' << monsterArray[i].speed.sides << '\n';

      //print abilities
      cout << monsterArray[i].ability << '\n';

      //print HP
      cout << monsterArray[i].HP.base << '+' << monsterArray[i].HP.dice << 'd' << monsterArray[i].HP.sides << '\n';

      //print AD
      cout << monsterArray[i].AD.base << '+' << monsterArray[i].AD.dice << 'd' << monsterArray[i].AD.sides << '\n';

      //print rarity
      cout << monsterArray[i].rarity << '\n';

      cout << '\n';
    }
     return 0;
  
  /* Default behavior: Seed with the time, generate a new dungeon, *
   * and don't write to disk.                                      */
  do_load = do_save = do_image = do_save_seed = do_save_image = 0;
  do_seed = 1;
  save_file = load_file = NULL;
  d.max_monsters = MAX_MONSTERS;

  /* The project spec requires '--load' and '--save'.  It's common  *
   * to have short and long forms of most switches (assuming you    *
   * don't run out of letters).  For now, we've got plenty.  Long   *
   * forms use whole words and take two dashes.  Short forms use an *
    * abbreviation after a single dash.  We'll add '--rand' (to     *
   * specify a random seed), which will take an argument of it's    *
   * own, and we'll add short forms for all three commands, '-l',   *
   * '-s', and '-r', respectively.  We're also going to allow an    *
   * optional argument to load to allow us to load non-default save *
   * files.  No means to save to non-default locations, however.    *
   * And the final switch, '--image', allows me to create a dungeon *
   * from a PGM image, so that I was able to create those more      *
   * interesting test dungeons for you.                             */
 
 if (argc > 1) {
    for (i = 1, long_arg = 0; i < argc; i++, long_arg = 0) {
      if (argv[i][0] == '-') { /* All switches start with a dash */
        if (argv[i][1] == '-') {
          argv[i]++;    /* Make the argument have a single dash so we can */
          long_arg = 1; /* handle long and short args at the same place.  */
        }
        switch (argv[i][1]) {
        case 'n':
          if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-nummon")) ||
              argc < ++i + 1 /* No more arguments */ ||
              !sscanf(argv[i], "%hu", &d.max_monsters)) {
            usage(argv[0]);
          }
          break;
        case 'r':
          if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-rand")) ||
              argc < ++i + 1 /* No more arguments */ ||
              !sscanf(argv[i], "%lu", &seed) /* Argument is not an integer */) {
            usage(argv[0]);
          }
          do_seed = 0;
          break;
        case 'l':
          if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-load"))) {
            usage(argv[0]);
          }
          do_load = 1;
          if ((argc > i + 1) && argv[i + 1][0] != '-') {
            /* There is another argument, and it's not a switch, so *
             * we'll treat it as a save file and try to load it.    */
            load_file = argv[++i];
          }
          break;
        case 's':
          if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-save"))) {
            usage(argv[0]);
          }
          do_save = 1;
          if ((argc > i + 1) && argv[i + 1][0] != '-') {
            /* There is another argument, and it's not a switch, so *
             * we'll save to it.  If it is "seed", we'll save to    *
	     * <the current seed>.rlg327.  If it is "image", we'll  *
	     * save to <the current image>.rlg327.                  */
	    if (!strcmp(argv[++i], "seed")) {
	      do_save_seed = 1;
	      do_save_image = 0;
	    } else if (!strcmp(argv[i], "image")) {
	      do_save_image = 1;
	      do_save_seed = 0;
	    } else {
	      save_file = argv[i];
	    }
          }
          break;
        case 'i':
          if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-image"))) {
            usage(argv[0]);
          }
          do_image = 1;
          if ((argc > i + 1) && argv[i + 1][0] != '-') {
            /* There is another argument, and it's not a switch, so *
             * we'll treat it as a save file and try to load it.    */
            pgm_file = argv[++i];
          }
          break;
        default:
          usage(argv[0]);
        }
      } else { /* No dash */
        usage(argv[0]);
      }
    }
  }

  if (do_seed) {
    /* Allows me to generate more than one dungeon *
     * per second, as opposed to time().           */
    gettimeofday(&tv, NULL);
    seed = (tv.tv_usec ^ (tv.tv_sec << 20)) & 0xffffffff;
  }

  srand(seed);

  io_init_terminal();
  init_dungeon(&d);

  if (do_load) {
    read_dungeon(&d, load_file);
  } else if (do_image) {
    read_pgm(&d, pgm_file);
  } else {
    gen_dungeon(&d);
  }

  /* Ignoring PC position in saved dungeons.  Not a bug. */
  config_pc(&d);
  gen_monsters(&d);

  io_display(&d);
  if (!do_load && !do_image) {
    io_queue_message("Seed is %u.", seed);
  }
  while (pc_is_alive(&d) && dungeon_has_npcs(&d) && !d.quit) {

    //if(d.teleport == 0){
      do_moves(&d);
      //}
    
  }
  io_display(&d);

  io_reset_terminal();

  if (do_save) {
    if (do_save_seed) {
       /* 10 bytes for number, plus dot, extention and null terminator. */
      save_file = (char *)malloc(18);
      sprintf(save_file, "%ld.rlg327", seed);
    }
    if (do_save_image) {
      if (!pgm_file) {
	fprintf(stderr, "No image file was loaded.  Using default.\n");
	do_save_image = 0;
      } else {
	/* Extension of 3 characters longer than image extension + null. */
	save_file = (char *)malloc(strlen(pgm_file) + 4);
	strcpy(save_file, pgm_file);
	strcpy(strchr(save_file, '.') + 1, "rlg327");
      }
    }
    write_dungeon(&d, save_file);

    if (do_save_seed || do_save_image) {
      free(save_file);
    }
  }

  printf("%s", pc_is_alive(&d) ? victory : tombstone);
  printf("You defended your life in the face of %u deadly beasts.\n"
         "You avenged the cruel and untimely murders of %u "
         "peaceful dungeon residents.\n",
         d.pc.kills[kill_direct], d.pc.kills[kill_avenged]);

  pc_delete(d.pc.pc);

  delete_dungeon(&d);

  return 0;
}
