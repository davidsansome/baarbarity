PlayerTypes.HumanSheep = new PlayerType;
PlayerTypes.HumanSheep.name = "HumanSheep";
PlayerTypes.HumanSheep.welsh = false;
PlayerTypes.HumanSheep.human = true;

function testAbility()
{
}

var a = new Ability(PlayerTypes.HumanSheep);
a.name = "Test ability";
a.description = "Ability used to test my l33t scriptz."
a.cost = 2;
a.callback = testAbility;

//var r = new ResourceRequirement(a);
//r.value = 2;
