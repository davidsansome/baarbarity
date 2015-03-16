/***************************************************************************
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "welshman.h"
#include "modelloader.h"
#include "scripting/scriptmanager.h"

ObjectType* Welshman::s_objectType = NULL;


Welshman::Welshman(GameState* gameState)
	: WorldObject(gameState)
{
	if (s_objectType == NULL)
		s_objectType = ScriptManager::objectType("Welshman");
	
	m_model = ModelLoader::loadModel("TestModel", 1.0f);
	loadPortrait(":/data/portraits/welshman.jpg");
	
	m_name = randomName();
}


QString Welshman::randomName() const
{
	switch (qrand() % 600)
	{
		case 0: return "Aberthol"; break; case 1: return "Adda"; break; 
		case 2: return "Addolgar"; break; case 3: return "Adwr"; break; 
		case 4: return "Aedd"; break; case 5: return "Afal"; break; 
		case 6: return "Alawn"; break; case 7: return "Albanwr"; break; 
		case 8: return "Alwin"; break; case 9: return "Alwyn"; break; 
		case 10: return "Amathaon"; break; case 11: return "Amerawdwr"; break; 
		case 12: return "Amhar"; break; case 13: return "Amlawdd"; break; 
		case 14: return "Amren"; break; case 15: return "Amynedd"; break; 
		case 16: return "Amyneddgar"; break; case 17: return "Anarawd"; break; 
		case 18: return "Andras"; break; case 19: return "Andreas"; break; 
		case 20: return "Anfri"; break; case 21: return "Angawdd"; break; 
		case 22: return "Anghrist"; break; case 23: return "Angor"; break; 
		case 24: return "Anwas"; break; case 25: return "Anwell"; break; 
		case 26: return "Anwil"; break; case 27: return "Anwill"; break; 
		case 28: return "Anwir"; break; case 29: return "Anwyl"; break; 
		case 30: return "Anwyll"; break; case 31: return "Anynnawg"; break; 
		case 32: return "Anyon"; break; case 33: return "ApEvan"; break; 
		case 34: return "ApHarry"; break; case 35: return "ApHowell"; break; 
		case 36: return "ApMaddock"; break; case 37: return "ApOwen"; break; 
		case 38: return "ApRhys"; break; case 39: return "ApRoderick"; break; 
		case 40: return "Ardwyad"; break; case 41: return "ArgIwydd"; break; 
		case 42: return "Arian"; break; case 43: return "Arthur"; break; 
		case 44: return "Arthwr"; break; case 45: return "Arval"; break; 
		case 46: return "Arvel"; break; case 47: return "Arvil"; break; 
		case 48: return "Avaon"; break; case 49: return "Awstin"; break; 
		case 50: return "Badan"; break; case 51: return "Badden"; break; 
		case 52: return "Baddon"; break; case 53: return "Baeddan"; break; 
		case 54: return "Baeddan"; break; case 55: return "Banys"; break; 
		case 56: return "Barris"; break; case 57: return "Beda"; break; 
		case 58: return "Bedwyr"; break; case 59: return "Bedyw"; break; 
		case 60: return "BeliMawr"; break; case 61: return "Bendigeidfran"; break; 
		case 62: return "Berth"; break; case 63: return "Berwyn"; break; 
		case 64: return "Bevan"; break; case 65: return "Beven"; break; 
		case 66: return "Bevin"; break; case 67: return "Bevyn"; break; 
		case 68: return "Blathaon"; break; case 69: return "Bleidd"; break; 
		case 70: return "Bowen"; break; case 71: return "Brac"; break; 
		case 72: return "Brac"; break; case 73: return "Brad"; break; 
		case 74: return "Bradwen"; break; case 75: return "Bradwr"; break; 
		case 76: return "Braen"; break; case 77: return "Bran"; break; 
		case 78: return "Brathach"; break; case 79: return "Brian"; break; 
		case 80: return "Broderick"; break; case 81: return "Brynn"; break; 
		case 82: return "Brys"; break; case 83: return "BwIch"; break; 
		case 84: return "Cadarn"; break; case 85: return "Cadawg"; break; 
		case 86: return "Caddoc"; break; case 87: return "Caddock"; break; 
		case 88: return "Cade"; break; case 89: return "Cadel"; break; 
		case 90: return "Cadell"; break; case 91: return "Cadellin"; break; 
		case 92: return "Caden"; break; case 93: return "Cadman"; break; 
		case 94: return "Cadmon"; break; case 95: return "Cadwallen"; break; 
		case 96: return "Cadwgawn"; break; case 97: return "Cadwr"; break; 
		case 98: return "Cadwy"; break; case 99: return "Cadyryeith"; break; 
		case 100: return "Caerau"; break; case 101: return "CaerLlion"; break; 
		case 102: return "Cai"; break; case 103: return "Cain"; break; 
		case 104: return "Calcas"; break; case 105: return "CaledvwIch"; break; 
		case 106: return "Camedyr"; break; case 107: return "Cant"; break; 
		case 108: return "Caradawg"; break; case 109: return "Caradoc"; break; 
		case 110: return "Caradoc"; break; case 111: return "Caradog"; break; 
		case 112: return "Carey"; break; case 113: return "Carne"; break; 
		case 114: return "Cary"; break; case 115: return "Cas"; break; 
		case 116: return "Casnar"; break; case 117: return "Cass"; break; 
		case 118: return "Cassian"; break; case 119: return "Cassidy"; break; 
		case 120: return "Casswallawn"; break; case 121: return "Caswallon"; break; 
		case 122: return "Cawrdav"; break; case 123: return "Cedric"; break; 
		case 124: return "Cedrick"; break; case 125: return "Cedrik"; break; 
		case 126: return "Ceithin"; break; case 127: return "CIyde"; break; 
		case 128: return "CIywd"; break; case 129: return "Clud"; break; 
		case 130: return "Clust"; break; case 131: return "Cnychwr"; break; 
		case 132: return "Coch"; break; case 133: return "Coed"; break; 
		case 134: return "Conway"; break; case 135: return "Conway"; break; 
		case 136: return "Conyn"; break; case 137: return "Craddock"; break; 
		case 138: return "Cradoe"; break; case 139: return "Crist"; break; 
		case 140: return "Cubert"; break; case 141: return "Culhwch"; break; 
		case 142: return "Culvanawd"; break; case 143: return "Custenhin"; break; 
		case 144: return "Cymry"; break; case 145: return "Cynbal"; break; 
		case 146: return "Cynbel"; break; case 147: return "Cystennin"; break; 
		case 148: return "Dafydd"; break; case 149: return "Dafydd"; break; 
		case 150: return "Dai"; break; case 151: return "Dalldav"; break; 
		case 152: return "Daned"; break; case 153: return "Daryn"; break; 
		case 154: return "Davies"; break; case 155: return "Davis"; break; 
		case 156: return "Davyn"; break; case 157: return "Delwin"; break; 
		case 158: return "Delwyn"; break; case 159: return "Deverell"; break; 
		case 160: return "Deverril"; break; case 161: return "Dewey"; break; 
		case 162: return "Dewey"; break; case 163: return "Dewi"; break; 
		case 164: return "Dewitt"; break; case 165: return "Digon"; break; 
		case 166: return "Dilan"; break; case 167: return "Dillie"; break; 
		case 168: return "Dillon"; break; case 169: return "Dillus"; break; 
		case 170: return "Dirrnyg"; break; case 171: return "Donne"; break; 
		case 172: return "Dover"; break; case 173: return "Drem"; break; 
		case 174: return "Dremidydd"; break; case 175: return "Drew"; break; 
		case 176: return "Druce"; break; case 177: return "Drudwas"; break; 
		case 178: return "Druson"; break; case 179: return "Drych"; break; 
		case 180: return "Drystan"; break; case 181: return "Dryw"; break; 
		case 182: return "Drywsone"; break; case 183: return "Duach"; break; 
		case 184: return "Dylan"; break; case 185: return "Dyllan"; break; 
		case 186: return "Dyllon"; break; case 187: return "Dyvynarth"; break; 
		case 188: return "Dyvyr"; break; case 189: return "Dywel"; break; 
		case 190: return "Earwin"; break; case 191: return "Earwine"; break; 
		case 192: return "Edern"; break; case 193: return "Edmyg"; break; 
		case 194: return "Eiddoel"; break; case 195: return "Eiladar"; break; 
		case 196: return "Einian"; break; case 197: return "Einion"; break; 
		case 198: return "Elphin"; break; case 199: return "Emhyr"; break; 
		case 200: return "EmIyn"; break; case 201: return "Emlyn"; break; 
		case 202: return "Emrys"; break; case 203: return "Erbin"; break; 
		case 204: return "ErcwIff"; break; case 205: return "Ergyryad"; break; 
		case 206: return "Eriyn"; break; case 207: return "Ermid"; break; 
		case 208: return "Erwin"; break; case 209: return "Erwyn"; break; 
		case 210: return "Eryi"; break; case 211: return "Eudav"; break; 
		case 212: return "Eus"; break; case 213: return "Evan"; break; 
		case 214: return "Evan"; break; case 215: return "Evann"; break; 
		case 216: return "Evin"; break; case 217: return "Evon"; break; 
		case 218: return "Evrawg"; break; case 219: return "Evyn"; break; 
		case 220: return "Fane"; break; case 221: return "Fercos"; break; 
		case 222: return "Fferyll"; break; case 223: return "Fflergant"; break; 
		case 224: return "Fflewdwr"; break; case 225: return "Ffodor"; break; 
		case 226: return "Ffowc"; break; case 227: return "Floyd"; break; 
		case 228: return "Fnam"; break; case 229: return "Fychan"; break; 
		case 230: return "Fyrsil"; break; case 231: return "Gaius"; break; 
		case 232: return "Garanhon"; break; case 233: return "Garanwyn"; break; 
		case 234: return "Gareth"; break; case 235: return "Gareth"; break; 
		case 236: return "Garnoc"; break; case 237: return "Garnock"; break; 
		case 238: return "Garreth"; break; case 239: return "Garwyli"; break; 
		case 240: return "Gavan"; break; case 241: return "Gavan"; break; 
		case 242: return "Gavin"; break; case 243: return "Gavin"; break; 
		case 244: return "Gavyn"; break; case 245: return "Geraint"; break; 
		case 246: return "Gilbert"; break; case 247: return "Gilvaethwy"; break; 
		case 248: return "GIyn"; break; case 249: return "GIynn"; break; 
		case 250: return "Gleis"; break; case 251: return "Glenn"; break; 
		case 252: return "Glew"; break; case 253: return "Glinyeu"; break; 
		case 254: return "Gobrwy"; break; case 255: return "Goreu"; break; 
		case 256: return "Gormant"; break; case 257: return "Gorsedd"; break; 
		case 258: return "Govan"; break; case 259: return "Govannon"; break; 
		case 260: return "Gower"; break; case 261: return "Gowyr"; break; 
		case 262: return "Granwen"; break; case 263: return "Greid"; break; 
		case 264: return "Griff"; break; case 265: return "Griffen"; break; 
		case 266: return "Griffeth"; break; case 267: return "Griffin"; break; 
		case 268: return "Griffith"; break; case 269: return "Griffith"; break; 
		case 270: return "Grigor"; break; case 271: return "Gruddyeu"; break; 
		case 272: return "Gruffen"; break; case 273: return "Gruffin"; break; 
		case 274: return "Gruffudd"; break; case 275: return "Gruffyn"; break; 
		case 276: return "Grufydd"; break; case 277: return "Gryphin"; break; 
		case 278: return "Gryphon"; break; case 279: return "Gusg"; break; 
		case 280: return "Gwakhmai"; break; case 281: return "Gwalchmei"; break; 
		case 282: return "Gwalhaved"; break; case 283: return "Gwallawg"; break; 
		case 284: return "Gwallter"; break; case 285: return "Gwarthegydd"; break; 
		case 286: return "Gwawl"; break; case 287: return "Gwayne"; break; 
		case 288: return "Gweir"; break; case 289: return "Gwendelyn"; break; 
		case 290: return "Gwenwynwyn"; break; case 291: return "Gwern"; break; 
		case 292: return "Gwernach"; break; case 293: return "Gwevyl"; break; 
		case 294: return "Gwilenhin"; break; case 295: return "Gwilym"; break; 
		case 296: return "Gwitart"; break; case 297: return "Gwrddywal"; break; 
		case 298: return "Gwres"; break; case 299: return "Gwyddawg"; break; 
		case 300: return "Gwydion"; break; case 301: return "Gwydre"; break; 
		case 302: return "Gwydyon"; break; case 303: return "Gwyn"; break; 
		case 304: return "Gwyngad"; break; case 305: return "Gwynn"; break; 
		case 306: return "Gwyr"; break; case 307: return "Gwystyl"; break; 
		case 308: return "Gwythyr"; break; case 309: return "Hael"; break; 
		case 310: return "Heilyn"; break; case 311: return "HenBeddestyr"; break; 
		case 312: return "HenWas"; break; case 313: return "HenWyneb"; break; 
		case 314: return "Howel"; break; case 315: return "Howell"; break; 
		case 316: return "Howell"; break; case 317: return "Hu"; break; 
		case 318: return "Huabwy"; break; case 319: return "Huarwar"; break; 
		case 320: return "Hueil"; break; case 321: return "Huey"; break; 
		case 322: return "Hugh"; break; case 323: return "Hughie"; break; 
		case 324: return "Huw"; break; case 325: return "Hydd"; break; 
		case 326: return "Iago"; break; case 327: return "Iau"; break; 
		case 328: return "Iddawg"; break; case 329: return "Iddig"; break; 
		case 330: return "Ioan"; break; case 331: return "Iona"; break; 
		case 332: return "Iorwerth"; break; case 333: return "Iustig"; break; 
		case 334: return "Iwan"; break; case 335: return "Jesstin"; break; 
		case 336: return "Jestin"; break; case 337: return "Jeston"; break; 
		case 338: return "Kai"; break; case 339: return "Kay"; break; 
		case 340: return "Kei"; break; case 341: return "Keith"; break; 
		case 342: return "Kelli"; break; case 343: return "Kelyn"; break; 
		case 344: return "ken"; break; case 345: return "Kendric"; break; 
		case 346: return "Kendrick"; break; case 347: return "Kendrik"; break; 
		case 348: return "Kendrix"; break; case 349: return "Kenn"; break; 
		case 350: return "Kenrick"; break; case 351: return "Kenrik"; break; 
		case 352: return "Kent"; break; case 353: return "Kent"; break; 
		case 354: return "Kenyon"; break; case 355: return "Kevyn"; break; 
		case 356: return "Kian"; break; case 357: return "Kilydd"; break; 
		case 358: return "Kim"; break; case 359: return "Kimball"; break; 
		case 360: return "Kimble"; break; case 361: return "Kyledyr"; break; 
		case 362: return "Kynan"; break; case 363: return "KyndMryn"; break; 
		case 364: return "Kynedyr"; break; case 365: return "Kynlas"; break; 
		case 366: return "Kynon"; break; case 367: return "Kynwal"; break; 
		case 368: return "Kynwyl"; break; case 369: return "Kywrkh"; break; 
		case 370: return "Lew"; break; case 371: return "Lewellyn"; break; 
		case 372: return "Llacheu"; break; case 373: return "Llara"; break; 
		case 374: return "Llassar"; break; case 375: return "Llawr"; break; 
		case 376: return "Llevelys"; break; case 377: return "LlewelIyn"; break; 
		case 378: return "Llewellyn"; break; case 379: return "Llewellyn"; break; 
		case 380: return "Llewelyn"; break; case 381: return "Lloyd"; break; 
		case 382: return "Lloyd"; break; case 383: return "Lludd"; break; 
		case 384: return "Llundein"; break; case 385: return "Llwybyr"; break; 
		case 386: return "Llwyd"; break; case 387: return "Llwydeu"; break; 
		case 388: return "Llwyr"; break; case 389: return "Llyn"; break; 
		case 390: return "Llyr"; break; case 391: return "Llyweilun"; break; 
		case 392: return "Luc"; break; case 393: return "Lug"; break; 
		case 394: return "Mabon"; break; case 395: return "Mabsant"; break; 
		case 396: return "Macsen"; break; case 397: return "Madawc"; break; 
		case 398: return "Madawg"; break; case 399: return "Maddoc"; break; 
		case 400: return "Maddock"; break; case 401: return "Maddock"; break; 
		case 402: return "Maddockson"; break; case 403: return "Maddocson"; break; 
		case 404: return "Maddog"; break; case 405: return "Maddox"; break; 
		case 406: return "Madoc"; break; case 407: return "Madog"; break; 
		case 408: return "Mael"; break; case 409: return "Maelgwyn"; break; 
		case 410: return "Maelwys"; break; case 411: return "Mallolwch"; break; 
		case 412: return "Manawydan"; break; case 413: return "March"; break; 
		case 414: return "Mathias"; break; case 415: return "Mawrth"; break; 
		case 416: return "Medyr"; break; case 417: return "Meilyg"; break; 
		case 418: return "Menw"; break; case 419: return "Mercher"; break; 
		case 420: return "Meredith"; break; case 421: return "Meredydd"; break; 
		case 422: return "Meridith"; break; case 423: return "Merlin"; break; 
		case 424: return "Merlyn"; break; case 425: return "Merrick"; break; 
		case 426: return "Mervin"; break; case 427: return "Mervyn"; break; 
		case 428: return "Merwyn"; break; case 429: return "Meurig"; break; 
		case 430: return "Mihangel"; break; case 431: return "Mil"; break; 
		case 432: return "Moesen"; break; case 433: return "Morcan"; break; 
		case 434: return "Morcar"; break; case 435: return "Mordwywr"; break; 
		case 436: return "Moren"; break; case 437: return "Morgan"; break; 
		case 438: return "Morgannwg"; break; case 439: return "Morgen"; break; 
		case 440: return "Morthwyl"; break; case 441: return "Morvran"; break; 
		case 442: return "Myrddin"; break; case 443: return "Myrick"; break; 
		case 444: return "Naw"; break; case 445: return "Neb"; break; 
		case 446: return "Neifion"; break; case 447: return "Nerth"; break; 
		case 448: return "Nerthach"; break; case 449: return "Neued"; break; 
		case 450: return "NewIyn"; break; case 451: return "Newlin"; break; 
		case 452: return "NewyddIlyn"; break; case 453: return "Nissyen"; break; 
		case 454: return "Nynnyaw"; break; case 455: return "Odgar"; break; 
		case 456: return "Ofydd"; break; case 457: return "Ol"; break; 
		case 458: return "Olwydd"; break; case 459: return "Ondyaw"; break; 
		case 460: return "Oswallt"; break; case 461: return "Owain"; break; 
		case 462: return "Owein"; break; case 463: return "Owen"; break; 
		case 464: return "Owin"; break; case 465: return "Owyn"; break; 
		case 466: return "Owynn"; break; case 467: return "Padrig"; break; 
		case 468: return "Pany"; break; case 469: return "Pedr"; break; 
		case 470: return "Peissawg"; break; case 471: return "Pembroke"; break; 
		case 472: return "PenIlyn"; break; case 473: return "Penn"; break; 
		case 474: return "Penvro"; break; case 475: return "Peredur"; break; 
		case 476: return "Perry"; break; case 477: return "Plys"; break; 
		case 478: return "Powell"; break; case 479: return "Preece"; break; 
		case 480: return "Price"; break; case 481: return "Pryce"; break; 
		case 482: return "Pryderi"; break; case 483: return "Prydwen"; break; 
		case 484: return "Puw"; break; case 485: return "Pwyll"; break; 
		case 486: return "Pyrs"; break; case 487: return "Reece"; break; 
		case 488: return "Rees"; break; case 489: return "Reese"; break; 
		case 490: return "Ren"; break; case 491: return "Renfrew"; break; 
		case 492: return "Rheged"; break; case 493: return "Rhett"; break; 
		case 494: return "Rhett"; break; case 495: return "Rhinffrew"; break; 
		case 496: return "Rhioganedd"; break; case 497: return "Rhisiart"; break; 
		case 498: return "Rhobert"; break; case 499: return "Rhun"; break; 
		case 500: return "Rhuvawn"; break; case 501: return "Rhyawdd"; break; 
		case 502: return "Rhychdir"; break; case 503: return "Rhyd"; break; 
		case 504: return "Rhys"; break; case 505: return "Rice"; break; 
		case 506: return "Romney"; break; case 507: return "Romney"; break; 
		case 508: return "Rumenea"; break; case 509: return "Ryn"; break; 
		case 510: return "Sadwm"; break; case 511: return "Saer"; break; 
		case 512: return "Saith"; break; case 513: return "Sayer"; break; 
		case 514: return "Sayers"; break; case 515: return "Sayre"; break; 
		case 516: return "Sayres"; break; case 517: return "Seith"; break; 
		case 518: return "Sel"; break; case 519: return "Selyf"; break; 
		case 520: return "Selyv"; break; case 521: return "Siarl"; break; 
		case 522: return "Siawn"; break; case 523: return "Siencyn"; break; 
		case 524: return "Sinnoch"; break; case 525: return "Sion"; break; 
		case 526: return "Sior"; break; case 527: return "Steffan"; break; 
		case 528: return "Steffen"; break; case 529: return "Steffon"; break; 
		case 530: return "Sugyn"; break; case 531: return "Sulyen"; break; 
		case 532: return "Syvwkh"; break; case 533: return "Tad"; break; 
		case 534: return "Tadd"; break; case 535: return "Taffy"; break; 
		case 536: return "Taliesin"; break; case 537: return "Taliesin"; break; 
		case 538: return "Taliesin"; break; case 539: return "Talyessin"; break; 
		case 540: return "Taren"; break; case 541: return "Tarran"; break; 
		case 542: return "Tarrant"; break; case 543: return "Taryn"; break; 
		case 544: return "Teithi"; break; case 545: return "Teregud"; break; 
		case 546: return "Tewdwr"; break; case 547: return "Timotheus"; break; 
		case 548: return "Tomos"; break; case 549: return "Trahaym"; break; 
		case 550: return "Trahern"; break; case 551: return "Tramaine"; break; 
		case 552: return "Traveon"; break; case 553: return "Travion"; break; 
		case 554: return "Travon"; break; case 555: return "Tremain"; break; 
		case 556: return "Tremaine"; break; case 557: return "Tremayne"; break; 
		case 558: return "Tremen"; break; case 559: return "Trent"; break; 
		case 560: return "Trevan"; break; case 561: return "Trevelian"; break; 
		case 562: return "Trevelian"; break; case 563: return "Trevelyan"; break; 
		case 564: return "Treven"; break; case 565: return "Trevian"; break; 
		case 566: return "Trevion"; break; case 567: return "Trevls"; break; 
		case 568: return "Trevonn"; break; case 569: return "Trevor"; break; 
		case 570: return "Trevyn"; break; case 571: return "Tringad"; break; 
		case 572: return "Tristan"; break; case 573: return "Tristram"; break; 
		case 574: return "Tristram"; break; case 575: return "Tristyn"; break; 
		case 576: return "Trynt"; break; case 577: return "Trynt"; break; 
		case 578: return "Trystan"; break; case 579: return "Tudor"; break; 
		case 580: return "Tudor"; break; case 581: return "Twm"; break; 
		case 582: return "Twrgadarn"; break; case 583: return "Tywysog"; break; 
		case 584: return "Uchdryd"; break; case 585: return "Vaddon"; break; 
		case 586: return "Vaughan"; break; case 587: return "Vaughn"; break; 
		case 588: return "Vonn"; break; case 589: return "Vontell"; break; 
		case 590: return "Vychan"; break; case 591: return "Wadu"; break; 
		case 592: return "Waljan"; break; case 593: return "Winn"; break; 
		case 594: return "Wmffre"; break; case 595: return "Wren"; break; 
		case 596: return "Wyn"; break; case 597: return "Wynn"; break; 
		case 598: return "Yale"; break; case 599: return "Ysberin"; break;
	}
	return QString::null;
}

QDataStream& operator >>(QDataStream& stream, Welshman& welshman)
{
	welshman.load(stream);
	return stream;
}

void Welshman::save(QDataStream& stream) const
{
	WorldObject::save(stream);
	
	stream << m_name;
}

void Welshman::load(QDataStream& stream)
{
	WorldObject::load(stream);
	
	stream >> m_name;
}


