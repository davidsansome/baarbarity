ObjectTypes.Building = new ObjectType;
ObjectTypes.Building.actionCompletedCallback = function(lastAction)
{
	if (lastAction instanceof ConstructionAction)
	{
		lastAction.billboard.enabled = true;
		lastAction.billboard.description = "Click and drag this icon to place the building"
	}
}

BuildingTypes.Relic = new BuildingType;
BuildingTypes.Relic.name = "Holy Relic";
BuildingTypes.Relic.description = "Blah";

BuildingTypes.ShearingShed = new BuildingType;
BuildingTypes.ShearingShed.name = "Shearing Shed";
BuildingTypes.ShearingShed.description = "Occasionally shears sheep within 20 meters producing wool.";



var a = new Ability(BuildingTypes.Relic);
a.name = "Build Shearing Shed";
a.description = "Starts building a shed that will shear nearby sheep and produce wool.";
a.cost = 1;
a.callback = function()
{
	var billboard = new Billboard(this);
	billboard.loadTexture(":/data/icons/placeholder.png");
	billboard.name = "Shearing shed";
	billboard.description = "This building is currently being built...";
	billboard.distance = 20;
	billboard.terrainValidTarget = true;
	billboard.droppedCallback = function(target)
	{
		if (target instanceof vec2)
		{
			var shed = new Building(Engine.state);
			shed.pos = target;
			shed.player = Engine.state.humanPlayer;
			shed.buildingType = BuildingTypes.ShearingShed;
			shed.finishedInitialisation();
		}
	}
	
	var a = new ConstructionAction(this);
	a.billboard = billboard;
	a.name = "Building shearing shed";
	a.duration = 1;
	
	this.action = a;
}
