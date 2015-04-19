object the_owner;
string cap_name;

get() {
    the_owner = this_player();
    cap_name = call_other(this_player(), "query_name", 0);
    return 1;
}

/* query_auto_load() {return "local/addendum:";} */

drop() { return 2; }

id(str) { return str == "addendum"; }

short() {return "WhiteQueen's famous all-purpose soul addendum";}

long() {
    write("This soul addendum enables you to use the following commands:\n"+
          "ponder, hit, hit <name>, ground, wthink, faint, wpray, recite,\n"+
          "wave, wave <name>, grovel, grovel <name>, pat <name>, fume,\n"+
          "wbounce, anger, stomp, stomp <name>, hkiss <name>, ack,\n"+    
          "happy, listen, listen <name>, watch, watch <name>, curious,\n"+
          "disgust, disgust <name>, face, face <name>, stroke <name>, \n"+
          "twirl, hover, quack, wworship <name>, surprise, surprise <name>,\n"+
          "nshock, nshock <name>, howl, snarl, recoil, tired, snigger,\n"+
          "snigger <name>, splutter, gurgle, congratulate <name>,\n"+
          "hnod, confused, smackeroo, wpolish, spolish, march, drill,\n"+
          "attention, ease, pushups, and cringe.\n");
}

ghost() {
    the_owner = this_player();
    cap_name = call_other(this_player(), "query_name", 0);
    return call_other(this_player(), "query_ghost");
}

init() {
    the_owner = this_player();
    cap_name = call_other(this_player(), "query_name", 0);
    add_action("ponder","ponder");
    add_action("hit","hit");
    add_action("ground","ground");
    add_action("wthink","wthink");
    add_action("faint","faint");
    add_action("wpray","wpray");
    add_action("recite","recite");   
    add_action("wave","wave");
    add_action("grovel","grovel");
    add_action("pat","pat");
    add_action("fume","fume");
    add_action("wbounce","wbounce");
    add_action("anger","anger");
    add_action("stomp","stomp");
    add_action("hkiss","hkiss");
    add_action("finger","finger");
    add_action("ack","ack");
    add_action("happy","happy");
    add_action("listen","listen");
    add_action("watch","watch");
    add_action("curious","curious");
    add_action("disgust","disgust");
    add_action("face","face");
    add_action("stroke","stroke");
    add_action("cello","cello");
    add_action("wworship","wworship");
    add_action("trumpet","trumpet");
    add_action("twirl","twirl");
    add_action("hover","hover");
    add_action("quack","quack");
    add_action("surprise","surprise");
    add_action("nshock","nshock");
    add_action("howl","howl");
    add_action("snarl","snarl");
    add_action("cringe","cringe");
    add_action("recoil","recoil");
    add_action("snigger","snigger");
    add_action("tired","tired");
    add_action("wcurtsey","wcurtsey");
    add_action("gurgle","gurgle");
    add_action("splutter","splutter");
    add_action("congratulate","congratulate");
    add_action("hnod","hnod");
    add_action("confused","confused");
    add_action("smackeroo","smackeroo");
    add_action("salute","salute");
    add_action("attention","attention");
    add_action("ease","ease");
    add_action("spolish","spolish");    
    add_action("wpolish","wpolish");     
    add_action("pushups","pushups");
    add_action("march","march");
    add_action("drill","drill");
    add_action("catchbreath","catchbreath");
}  

     
ponder() {
    if (ghost())
	return 0;
    write("You ponder over the troubles of the world.\n");
    say(cap_name + " ponders over some unknown trouble.\n");
    return 1;
}

hit(str) {
    object who;
    if (ghost())
	return 0;
    if (!str) {
        write("You wave your fists in the air.\n");
	say(cap_name + " practises shadow boxing.\n");
	return 1;
    }
    who = present(lower_case(str), environment(this_player()));
    if (!who || !living(who) || who == this_player())
	return 0;
    tell_object(who, cap_name + " gives you an uppercut.\n");
    write("You give " + capitalize(str) +" an uppercut.\n");
    say(cap_name + " gives " + capitalize(str) + " an uppercut.\n", who);
    return 1;
}

ground() {
    if (ghost())
	return 0;
    write("You hit the ground with a powerful thud.\n");
    say(cap_name + " lands on the ground with a powerful thud.\n");
    return 1;
}

wthink() {
    if (ghost())
	return 0;
    write("You try to use your brain but fail.\n");
    say(cap_name + " tries to think but fails.\n");
    return 1;
}

faint() {
    if (ghost())
	return 0;
    write("Everything seems to spin, then it goes black.\n"+
          "You wake up presently, feeling a bit lost.\n");
    say(cap_name + " looks pale and faints, but regains consciousness quickly.\n");
    return 1;
}

wpray() {
    if (ghost())
        return 0;
    write("You feel more hopeful as you kneel and pray.\n");
    say(cap_name + " kneels and prays.\n");
    return 1;
}

recite() {
    if (ghost())
        return 0;
    write("To play MUD, or not to play MUD???.... that is the question.\n");
    say(cap_name + " recites a Shakespearean monologue with a strange twist.\n");
    return 1;
}


wave(str) {
    object who;
    if (ghost())
	return 0;
    if (!str) {
        write("You wave at your audience.\n");
	say(cap_name + " waves at everybody.\n");
	return 1;
}
    who = present(lower_case(str), environment(this_player()));

    if (!who || !living(who) || who == this_player())
	return 0;
    tell_object(who, cap_name + " waves at you.\n");
    write("You wave at " + capitalize(str) +".\n");
    say(cap_name + " waves at " + capitalize(str) + ".\n", who);
    return 1;
}

grovel(str) {
    object who;
    if (ghost())
        return 0;
    if (!str) {
        write("You grovel in the mud.\n");
        say(cap_name + " grovels to no one in particular.\n");
        return 1;
    }
    who = present(lower_case(str), environment(this_player()));
    if (!who || !living(who) || who == this_player())    
	return 0;
    tell_object(who, cap_name + " grovels to you.\n");
    write("You grovel to " + capitalize(str) + ".\n");
    say(cap_name + " grovels to " + capitalize(str) + ".\n", who);
    return 1;
}

pat(str) {
    object who;
    if (ghost())
        return 0;
    if (!str) {
        write("You can't pat thin air on the head!\n");
        say(cap_name + " tries to pat thin air on the head... silly, huh?\n");
        return 1;
    }
    who = present(lower_case(str), environment(this_player()));
    if (!who || !living(who) || who == this_player())
        return 0;
    tell_object(who, cap_name + " pats you on the head.\n");
    write("You pat " + capitalize(str) + " on the head.\n");
    say(cap_name + " pats " + capitalize(str) + " on the head.\n", who);
    return 1;
}

fume() {
    if (ghost())
        return 0;
    write("You puff and fume like a compulsive smoker.\n");
    say(cap_name + " starts to puff and fume.\n");
    return 1;
}

wbounce() {
    if(ghost())
       return 0;
    write("Dignified, graceful B O I N G s resound!\n");
    say(cap_name + " bounces around with grace and dignity.\n");
    return 1;
}

anger() {
    if(ghost())
       return 0;
    write("You jump up and down in anger!\n");
    say(cap_name + " jumps up and down in anger!\n");
    return 1;
}

stomp(str) {
    object who;
    if (ghost())
	return 0;
    if (!str) {
        write("You stomp on the ground like a furious elephant.\n");
        say(cap_name + " stomps on the ground like a furious elephant.\n");
	return 1;
}
    who = present(lower_case(str), environment(this_player()));

    if (!who || !living(who) || who == this_player())
	return 0;
    tell_object(who, cap_name + " stomps on your foot.  OW!\n");
    write("You stomp on " + capitalize(str) +"'s foot.\n");
    say(cap_name + " stomps on " + capitalize(str) + "'s foot.\n", who);
    return 1;
}

hkiss(str) {
    object who;
    if (ghost())
	return 0;
    if (!str) {
        write("You can't kiss a hand when nobody is here!\n");
	return 1;
}
    who = present(lower_case(str), environment(this_player()));

    if (!who || !living(who) || who == this_player())
	return 0;
    tell_object(who, cap_name + " kisses your hand.\n");
    write("You kiss " + capitalize(str) +"'s hand.\n");
    say(cap_name + " kisses " + capitalize(str) + "'s hand.\n", who);
    return 1;
}

finger(str) {
    object who;
    if (ghost())
	return 0;
    if (!str) {
        write("Like the obscene character you are... Shame on you!\n");
        say(cap_name + " shows everyone an obscene gesture.\n");
	return 1;
    }
    who = present(lower_case(str), environment(this_player()));

    if (!who || !living(who) || who == this_player())
	return 0;
    tell_object(who, cap_name + " gives you the finger.\n");
    write("You give " + capitalize(str) +" the finger.\n");
    say(cap_name + " gives " + capitalize(str) + " the finger.\n", who);
    return 1;
}

ack() {
    if (ghost())
        return 0;
    write("You cough from a hair ball just like Bill the Cat!.\n");
    say(cap_name + " sounds like Bill the Cat.\n");
    return 1;
}

happy() {
    if (ghost())
        return 0;
    write("You feel deliriously happy.\n");
    say(cap_name + " looks deliriously happy.\n");
    return 1;
}

listen(str) {
    object who;
    if (ghost())
	return 0;
    if (!str) {
        write("You listen intently.\n");
        say(cap_name + " listens intently.\n");
	return 1;
    }
    who = present(lower_case(str), environment(this_player()));

    if (!who || !living(who) || who == this_player())
	return 0;
    tell_object(who, cap_name + " listens to you.\n");
    write("You listen to what " + capitalize(str) +" has to say.\n");
    say(cap_name + " listens to what " + capitalize(str) + " has to say.\n", who);
    return 1;
}

watch(str) {
    object who;
    if (ghost())
	return 0;
    if (!str) {
        write("You watch the surroundings carefully.\n");
        say(cap_name + " watches the surroundings carefully.\n");
	return 1;
    }
    who = present(lower_case(str), environment(this_player()));

    if (!who || !living(who) || who == this_player())
	return 0;
    tell_object(who, cap_name + " watches your every move!\n");
    write("You watch " + capitalize(str) +"'s every move!\n");
    say(cap_name + " watches " + capitalize(str) + "'s every move!\n", who);
    return 1;
}

curious() {
    if (ghost())
    return 0;
    write("You feel very curious, and no doubt look like it, too!\n");
    say(cap_name + " looks curious.\n");
    return 1;
}

disgust(str){ 
    object who;
    if (ghost())
	return 0;
    if (!str) {
        write("You look very disgusted.\n");
        say(cap_name + " looks very disgusted.\n");
	return 1;
    }
    who = present(lower_case(str), environment(this_player()));

    if (!who || !living(who) || who == this_player())
	return 0;
    tell_object(who, cap_name + " looks at you in disgust.\n");
    write("You look at " + capitalize(str) +" in disgust.\n");
    say(cap_name + " looks at " + capitalize(str) + " in disgust.\n", who);
    return 1;
}

face(str){ 
    object who;
    if (ghost())
	return 0;
    if (!str) {
        write("You make a terrible face.\n");
        say(cap_name + " makes a terrible face.\n");
	return 1;
    }
    who = present(lower_case(str), environment(this_player()));

    if (!who || !living(who) || who == this_player())
	return 0;
    tell_object(who, cap_name + " says 'Bleah!' and makes an awful face at you.\n");
    write("You make an awful face at " + capitalize(str) +".\n");
    say(cap_name + " says 'Bleah!' and makes an awful face at " + capitalize(str) + ".\n", who);
    return 1;
}

stroke(str){ 
    object who;
    if (ghost())
	return 0;
    if (!str) {
        write("How silly of you to try and stroke thin air on the cheek!\n");
        say(cap_name + " makes a few strokes in thin air.\n");
	return 1;
    }
    who = present(lower_case(str), environment(this_player()));

    if (!who || !living(who) || who == this_player())
	return 0;
    tell_object(who, cap_name + " strokes your cheek lovingly.\n");
    write("You stroke " + capitalize(str) +"'s cheek lovingly.\n");
    say(cap_name + " strokes " + capitalize(str) + "'s cheek lovingly.\n", who);
    return 1;
}

cello() {
    if (ghost())
    return 0;
    write("You play a Bach solo suite on the cello as if by magic.\n");
    say(cap_name + " plays a Bach solo suite on the cello.\n");
    return 1;
}

wworship(str){ 
    object who;
    if (ghost())
	return 0;
    if (!str) {
        write("Feels silly to worship the ground, doesn't it?\n");
        say(cap_name + " worships the ground.\n");
	return 1;
    }
    who = present(lower_case(str), environment(this_player()));

    if (!who || !living(who) || who == this_player())
	return 0;
    tell_object(who, cap_name + " worships you.\n");
    write("You worship " + capitalize(str) +".\n");
    say(cap_name + " worships " + capitalize(str) + ".\n", who);
    return 1;
}

trumpet() {
    if (ghost())
    return 0;
    write("You jazz away just like Louis Armstrong!\n");
    say(cap_name + " jazzes away on the trumpet like Louis Armstrong.\n");
    return 1;
}

twirl() {
    if (ghost())
    return 0;
    write("You are a walking tornado.\n");
    say(cap_name + " twirls and looks like a walking tornado.\n");
    return 1;
}

hover() {
    if (ghost())
    return 0;
    write("You hover like a ghost even though you are not one!\n");
    say(cap_name + " hovers in midair like a ghost.\n");
    return 1;
}

quack() {
    if (ghost())
    return 0;
    write("Quack!  QUACK! (What did you expect???)\n");
    say(cap_name + " sounds like Donald Duck.\n");
    return 1;
}

nshock(str) {
    object who;
    if (ghost())
    return 0;
    if(!str) {
       write("You shock yourself.\n");
       say(cap_name + " looks shocked.\n");
       return 1;
       }
       who = present(lower_case(str), environment(this_player()));
       if(!who || !living(who) || who == this_player())
          return 0;
       tell_object(who, cap_name + " shocks you immensely!\n");
       write("You shock " + capitalize(str) + " immensely.\n");
       say(cap_name + " shocks " + capitalize(str) + " immensely!\n",who);
       return 1;
}
 
surprise(str) {
     object who;
       if (ghost())
           return 0;
       if(!str) {
       write("You look surprised!\n");
       say(cap_name + " looks very surprised.\n");
       return 1;
       }
       who = present(lower_case(str), environment(this_player()));
       if(!who || !living(who) || who == this_player())
           return 0;
       tell_object(who, cap_name + " surprises you and you gasp in astonishment!\n");
       write("You surprise " + capitalize(str) + ", who gasps in astonishment.\n");
       say(cap_name + " surprises " + capitalize(str) + ", who gasps in astonishment!\n",who);
       return 1;
}

howl(){
     if(ghost())
        return 0;
     write("You howl in pain!\n");
     say(cap_name + " howls in pain.\n");
     return 1;
}

snarl() {
     if(ghost())
        return 0;
     write("You snarl between your teeth.\n");
     say(cap_name + " snarls menacingly.\n");
     return 1;
}

cringe() {
     if(ghost())
        return 0;
     write("You cringe in terror.\n");
     say(cap_name + " cringes in terror.\n");
     return 1;
}

recoil() {
     if(ghost())
        return 0;
     write("You recoil with fear.\n");
     say(cap_name + " recoils with fear.\n");
     return 1;
}

tired() {
     if(ghost())
        return 0;
     command("yawn", this_player());
     write("You feel very tired, as if you've just had an all-nighter.\n");
     say(cap_name + " looks very tired; it must have been a long night.\n"+
         "Perhaps it was even an all-nighter!\n");
     return 1;
}

wcurtsey() {
     if(ghost())
        return 0;
     write("You curtsey with royal grace, just like you should.\n");
     say("The White Queen curtseys with royal grace.\n");
     return 1;
}


snigger(str) {
     object who;
       if (ghost())
           return 0;
       if(!str) {
       write("You snigger jeeringly.\n");
       say(cap_name + " sniggers jeeringly.\n");
       return 1;
       }
       who = present(lower_case(str), environment(this_player()));
       if(!who || !living(who) || who == this_player())
           return 0;
       tell_object(who, cap_name + " sniggers jeeringly at you.\n");
       write("You snigger jeeringly at " + capitalize(str) + ".\n");
       say(cap_name + " sniggers jeeringly at " + capitalize(str) + ".\n",who);
       return 1;
}

gurgle() {
     if(ghost()) 
        return 0;
     write("You gurgle.\n");
     say(cap_name + " gurgles.\n");
     return 1;
}

splutter() {
     if(ghost())
        return 0;
     write("You splutter worse than a drenched cat!\n");
     say(cap_name + " splutters worse than a drenched cat!\n");
     return 1;
}

congratulate(str) {
     object who;
       if (ghost())
           return 0;
       if(!str) {
       write("You congratulate thin air.  How silly.\n");
       say(cap_name + " congratulates thin air.\n");
       return 1;
       }
       who = present(lower_case(str), environment(this_player()));
       if(!who || !living(who) || who == this_player())
           return 0;
       tell_object(who, cap_name + " congratulates you.\n");
       write("You congratulate " + capitalize(str) + ".\n");
       say(cap_name + " congratulates " + capitalize(str) + ".\n",who);
       return 1;
}

hnod(){
    if(ghost())
       return 0;
    write("You nod happily.\n");
    say(cap_name + " nods happily.\n");
    return 1;
}

confused(){
    if(ghost())
       return 0;
    write("You feel as confused as you look!\n");
    say(cap_name + " looks very confused.\n");
    return 1;
}

smackeroo(str){
        object who;
    if(ghost())
       return 0;
    if(!str){
       write("You blow everyone a big smackeroo.\n");
       say(cap_name + " blows everyone a big smackeroo.\n");
       return 1;
       }
    who = present(lower_case(str), environment(this_player()));
    if(!who || !living(who) || who == this_player())
       return 0;
    tell_object(who, cap_name + " plants a big smackeroo on your cheek.\n");
    write("You plant a big smackeroo on " + capitalize(str) + "'s cheek.\n");
    say(cap_name + " plants a big smackeroo on " + capitalize(str) + "'s cheek.\n",who);
    return 1;
}

march(){
    if(ghost())
       return 0;
    write("You march around.\n");
    say(cap_name + " marches around.\n");
    return 1;
}

drill(){
    if(ghost())
       return 0;
    write("You are a very well-trained soldier!\n");
    say(cap_name + " looks like a very well-trained soldier.\n");
    return 1;
}

wpolish(){
    if(ghost())
       return 0;
    write("You polish your weapon.\n");
    say(cap_name + "'s weapon shines as it is polished.\n");
    return 1;
}

salute(str){
        object who;
    if(ghost())
       return 0;
    if(!str){
       write("You salute your audience.\n");
       say(cap_name + " salutes everyone.\n");
       return 1;
       }
    who = present(lower_case(str), environment(this_player()));
    if(!who || !living(who) || who == this_player())
       return 0;
    tell_object(who, cap_name + " salutes you.\n");
    write("You salute " + capitalize(str) + ".\n");
    say(cap_name + " salutes " + capitalize(str) + ".\n",who);
    return 1;
}

spolish(){
    if(ghost())
       return 0;
    write("There is now no MUD on your shoes.  How depressing.\n");
    say(cap_name + "'s shoes are now well polished.\n");
    return 1;
}

pushups(){
    if(ghost())
       return 0;
    write("You feel more in shape as you do twenty push-ups:\n" +
          "Eee!\n"+
          "Yow!\n"+
          "Eee!\n"+
          "Yow!\n"+
          "Eee!\n"+
          "Yow!\n"+
          "Eee!\n"+
          "Yow!\n"+
          "Eee!\n"+
          "Yow!\n"+
          "Eee!\n"+
          "Yow!\n"+
          "Eee!\n"+
          "Yow!\n"+
          "Eee!\n"+
          "Yow!\n"+
          "Eee!\n"+
          "Yow!\n"+
          "Eee!\n"+
          "Yow!\n");
    say(cap_name + " does twenty push-ups.\n"+
          "And up...\n"+
          "and down...\n"+
          "And up...\n"+
          "and down...\n"+
          "And up...\n"+
          "and down...\n"+
          "And up...\n"+
          "and down...\n"+
          "And up...\n"+
          "and down...\n"+
          "And up...\n"+
          "and down...\n"+
          "And up...\n"+
          "and down...\n"+
          "And up...\n"+
          "and down...\n"+
          "And up...\n"+
          "and down...\n"+
          "And up...\n"+
          "and down...\n");
    command("ground",this_player());
    command("catchbreath",this_player());
    return 1;
}

attention(){
    if(ghost())
       return 0;
    write("You stand at attention.\n");
    say(cap_name + " stands at attention.\n");
    return 1;
}

ease(){
    if(ghost())
       return 0;
    write("You stand at ease.\n");
    say(cap_name + " stands at ease.\n");
    return 1;
}
    
catchbreath(){
    if(ghost())
       return 0;
    write("You catch your breath.  Phew!\n");
    say(cap_name + " catches some air again.\n");
    return 1;
}
