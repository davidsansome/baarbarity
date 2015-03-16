function sheepActionCompleted(lastAction)
{
	if (lastAction instanceof PlayerMoveAction)
	{
		var target = lastAction.followObject;
		
		if (!this.isMemberOfSquad() &&
		    target instanceof Sheep &&
		    (target.player == this.player || target.player == null) &&
		    !target.isSquadLeader())
		{
			// The player clicked on another sheep.
			// Add him to our squad
			
			target.player = this.player;
			target.squadLeader = this;
		}
		else if (this.isMemberOfSquad())
		{
			// Face the same way as our squad leader
			
			this.facing = this.squadLeader.facing;
		}
	}
	
    if (this.squadLeader == Sheep)
    {
        var a = new EatGrassAction(this);
        a.dest = new vec2(this.squadLeader.pos.x + randBetween(-4, 4), 
                          this.squadLeader.pos.y + randBetween(-4, 4));
        a.thinkingDelay = randBetween(7000, 15000);
        a.eatingDelay = randBetween(10000, 20000);
        this.action = a;
    }
}

function sheepContextAction(target)
{
	if (target instanceof vec2)
	{
		// The player has clicked on the terrain.  Move the sheep there.
		var a = new PlayerMoveAction(this);
		a.dest = target;
		
		this.action = a;
	}
	else if (target instanceof Sheep)
	{
		// The player has clicked on another sheep.
		// Follow the sheep, then do something.
		var a = new PlayerMoveAction(this);
		a.followObject = target;
		a.followDistance = 4.0;
		
		this.action = a;
	}
}

ObjectTypes.Sheep = new ObjectType;
ObjectTypes.Sheep.actionCompletedCallback = sheepActionCompleted;
ObjectTypes.Sheep.contextActionCallback = sheepContextAction;

