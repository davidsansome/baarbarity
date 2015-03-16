function welshmanActionCompleted(lastAction)
{
    if (lastAction instanceof PlayerMoveAction 
        && lastAction.followObject instanceof Sheep)
    {
        if (!(lastAction.followObject.squadLeader instanceof Welshman))
        {//Claim it for it's own use.
            this.victim = lastAction.followObject;
            this.victim.squadLeader = this;
            print(this.name + " has claimed " + this.victim.name);
        }
    }

	t = new Timer(this);
	t.interval = randBetween(4000,6000);
	t.callback = welshmanTick;
}

function welshmanTick()
{
    if (!(this.victim) && ( randBetween(0,1) > 0.9 ))
    {
        var nearestSheeps = this.objectsNear(20, ObjectTypes.Sheep, true);
        for (var i=0; i<nearestSheeps.length; i++)
        {
            if (!(nearestSheeps[i].squadLeader instanceof Welshman)){
                var a = new PlayerMoveAction(this);
                a.followObject = nearestSheeps[i];
                a.followDistance = 0.2;
                this.action = a;

                return false;
            }
        }
    }
    
    if ( this.victim && ( randBetween(0,1) > 0.95 ))
    {
        print(this.name + "is getting his raunch on")

        var a = new PlayerMoveAction(this.victim)
		a.dest = new vec2(this.pos.x + randBetween(-100, 100), 
		                  this.pos.y + randBetween(-100, 100));
		this.victim.action = a;
        this.victim.squadLeader = null;
        print(this.victim.name + " is running for its little fleecy weesy");
        this.victim = null
    }

	if (randBetween(0, 1) > 0.7)
	{
        print(this.name + "is going for a stroll");
		var a = new PlayerMoveAction(this);
		a.dest = new vec2(this.pos.x + randBetween(-10, 10), 
		                  this.pos.y + randBetween(-10, 10));
		this.action = a;

		return false;
	}
	return true;
}
ObjectTypes.Welshman = new ObjectType;
ObjectTypes.Welshman.actionCompletedCallback = welshmanActionCompleted;
