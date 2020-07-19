float() Pet_FindTarget =
{
	local entity	client;
	local float	r;
        local entity head, selected;
	local float dist;



        dist = 10000;
	selected = world;
	head = findradius(self.origin, 10000);
	while(head)
	{
		if( (head.health > 1) && (head != self) && (head != self.owner))
		{
			traceline(self.origin,head.origin,TRUE,self);
			if ( (trace_fraction >= 1) && (vlen(head.origin - self.origin) < dist) && (head.owner != self.owner))
			{
				selected = head;
				dist = vlen(head.origin - self.origin);
			}
		}
		head = head.chain;
	}
	if (selected != world)
	{
		sprint (self.owner,"Pet attacking -> ");
		if (selected.classname == "player")
		{
			sprint (self.owner,selected.netname);
			sprint (selected,self.owner.netname);
			sprint (selected," sent one of his minions after you!\n");
		}
		else
			sprint (self.owner,selected.classname);
		sprint (self.owner,"\n");
                self.enemy = selected;
                FoundTarget ();

                return TRUE;
	}


    if (self.goalentity != self.owner)
    {
        self.goalentity = self.owner;
        self.think = self.th_run;
    }
    self.ideal_yaw = vectoyaw(self.owner.origin - self.origin);
    self.nextthink = time+0.1;
    return FALSE;
};










