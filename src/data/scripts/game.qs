Engine.initGameCallback = function()
{
	// Make the players
	var cpu = new Player(Engine.state);
	cpu.type = PlayerTypes.CPUWelsh;
	cpu.name = "Welsh";
	cpu.color = new QColor(255, 128, 128);
	cpu.homeLocation = new vec2(Engine.state.terrain.width / 5.0,
	                            Engine.state.terrain.height / 5.0);
	
	Engine.state.humanPlayer = new Player(Engine.state);
	Engine.state.humanPlayer.type = PlayerTypes.HumanSheep;
	Engine.state.humanPlayer.name = "Bob";
	Engine.state.humanPlayer.color = new QColor(128, 128, 255);
	Engine.state.humanPlayer.homeLocation = new vec2(Engine.state.terrain.width / 5.0 * 4.0,
	                                                 Engine.state.terrain.height / 5.0 * 4.0);
	
	// Make 50 randomly positioned neutral sheep
	for (var i=0 ; i<50 ; i++)
	{
		var pos = new vec2(randBetween(0, Engine.state.terrain.width - 1),
		                   randBetween(0, Engine.state.terrain.height - 1));
		
		var sheep = new Sheep(Engine.state);
		sheep.pos = pos;
		sheep.finishedInitialisation();
	}

	// Make 10 welshman near the cpu's starting area
	for (var i=0 ; i<10 ; i++)
	{
		var pos = Engine.state.randomPositionNear(cpu.homeLocation, 12);
		
		var welshman = new Welshman(Engine.state);
		welshman.pos = pos;
		welshman.player = cpu;
		welshman.finishedInitialisation();
	}

	// Make a sheep near the player's starting area
	var pos = Engine.state.randomPositionNear(Engine.state.humanPlayer.homeLocation, 12);
	
	var startSheep = new Sheep(Engine.state);
	startSheep.pos = pos;
	startSheep.player = Engine.state.humanPlayer;
	startSheep.changeLoyalty(Engine.state.humanPlayer, 100);
	startSheep.finishedInitialisation();
	
	// Make the player's relic
	var relic = new Building(Engine.state);
	relic.pos = Engine.state.humanPlayer.homeLocation;
	relic.player = Engine.state.humanPlayer;
	relic.buildingType = BuildingTypes.Relic;
	relic.finishedInitialisation();
}

