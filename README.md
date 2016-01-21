# ZXCmod 1.32 free source 2008-2010.

For compile dll, need C++ 5 or 6 and Half-Life SDK 2.3  
Get current HL SDK [here](https://github.com/ValveSoftware/halflife). 

Visit [this](https://github.com/ZXCmod/ZXCmod-info) repo for read other information. 

#Changelog
 
###1.32

- fourth attack on RPG - (rpg.cpp, hornetgun.cpp, weapons.h)
- fourth attack on hornetgun: heal ray - (hornetgun.cpp, weapons.h)
- gauss magnet ray updated - (gauss.cpp)
- crossbow 4th attack updated - (crossbow.cpp)
- "DamageForce" changed from `5` to `3` - (combat.cpp)
  
  
###1.31 [3rd update] 

- third attack on crossbow
- fourth attack on crossbow
- improved weap. balance
- other little changes.

   
###1.31

- new function: `FourthAttack()`. Any effect by pressed `USE` and `ATTACK2` keys - (weapons.cpp, weapons.h)
- fourth attack on crowbar: power hit
- fourth attack on glock: zoom
- fourth attack on handgrenates
- fourth attack on satchels
- fourth attack on egon: storm
- fourth attack on shotgun: cannon
- fourth attack on mp5: plasma
- new console command: `mp_tank`. Default `0`.
- new console command: `mp_teleport`. Default `1`.
- weapon balance improved
- optimized: weapon code, source code, net game.
- some bugs destroyed, but hlds can crash with `sv_cheats 1` and `mp_teamplay 1`.
- cleared bad code in player part. [2nd upd] - (client.cpp, player.cpp)
- second satchels updated and crowbars (models replaced) [2nd upd] - (ggrenade.cpp, satchels.cpp, crowbar.cpp, weapons.cpp)

  
###1.30a

- third attack on tripmine: Ice-Friction, only heal crystal can return inital friction - (tripmine.cpp, satchel.cpp)
- new console command: `mp_python`. Value `0` is disabled black rocket - (game.cpp, python.cpp, game.h)
- new console command: `mp_gauss`. Value `0` is disabled gauss velocity - (game.cpp, gauss.cpp, game.h)
- included maps `2houses.bsp` and `2play.bsp` for teamplay
- hot bugs destroyed.

 
###1.30

- third attack on egon: Tesla - (egon.cpp)
- egon cannon now bounce - (egon.cpp) 
- added new function: `TeslaExplode()`, is spread shock streams - (combat.cpp, cbase.h)
- Added switcher - (turrets.cpp)
- teamplay improved
- improved weapon balance
- when `sv_cheats 1`, disabled : hornets and grav_grenades; turrets limit is `3` 
- some bugs from previous versions has been fixed.


###1.29

- third attack on snarks: stomp. - (squeakgrenade.cpp)
- third attack on RPG: dual missiles. - (rpg.cpp)
- new console command: `mp_dmode`. Default `0` - (game.cpp, game.h, player.cpp, world.cpp, shotgun.cpp)
- decrease damage x2 by yourself explosive - (combat.cpp)
- when `sv_cheats 1`, available all weapons on start 
- bad code cleanup
- include some maps
- other bug fixes, little changes. 


###1.28 
 
- third attack on mp5: lazers with little zap. - (mp5.cpp)
- third attack on hornetgun: freeze bomb. - (hornetgun.cpp)
- third attack on hand grenades: smoke - (handgrenade.cpp)
- new console command: `mp_crowbar`. Value `0` is disabled rockets, multiply dmg x2. Default 1 - (game.cpp, crowbar.cpp, game.h, shotgun.cpp)
- set 5 turrets limit - (turret.cpp, rpg.cpp)
- other bug fixes and changes. 


###1.27 
 
- third attack on glock: gravity-shock bullets - (hl_wpn_glock.cpp)
- third attack on crowbar: teleportation - (crowbar.cpp)
- third attack on gauss: electro-ray - (gauss.cpp)
- new console command: `mp_fragmonsters`. Allow any values. Default 1 - (game.cpp, combat.cpp, game.h)
- fixed: client-side animation disabled. - (client.cpp & anothers with FEV_)
- secondary and reload attacks on mortars - (func_tank.cpp)
- optimized shotgun clusters - (shotgun.cpp)
- other little changes. 


###v1.26

- New weapons and changes.
- Sentry cant stuck in floor and walls - (rpg.cpp)
- Nuke has more direct damage *10 - (gauss.cpp)
- added new weapon: analog rocket-crowbar from RC - (crowbar.cpp)
- increase freeze from `1.25` to `2.25` - (hornetgun.cpp)
- allow to freeze anybody monsters - (combat.cpp)
- new function: `ThirdAttack()`. Any effect by pressed `USE` and `ATTACK1` keys - (weapons.cpp, weapons.h)
- third attack n shotgun - (shotgun.cpp)
- third attack on satchels (invisibility) - (satchel.cpp, player.cpp)
- fixed crowbar animation                                                                             
- many other changes and release.


###v1.25

- New weapons: alt-nuke on egon (press reload) and freeze hornets (secondary attack on hornetgun).
- Rebalance of turrets. Sentry has 90% hp, turret - 100%. Low dmg.
- added "Headhot" message.


###v1.24

- New turret for Rocket launcher.
- small rebalance of turrets. Sentry has 100% hp, turret - 150%.
- turret and sentry are do not shot ally in `mp_teamplay 1`.
- some effects to players, machines and radiation. 


###v1.23
- NukeBomb explode fixed.
- Return autoset `cl_lw 1` for best fire prediction (must some bugs with animation hand weapons (i work on it)).
- added new weapon: Flash grenates on Hornetgun. Small damage, big explode. Press reload for attack.
- Many other changes.


###v1.22
- GravGrenade lags fixed.
- Removed autoset some console commands and `AUTOUPDATE`.
- added new weapon: Uranium Bullet on 357(press `RMB`, if you have 6 bullets for magnum).


###v1.21
- changed damage by some weapons.
- added new weapon: GravGrenade (press `R` if you have 5 handgrenades).
- more effects by nuke bomb.
- more small changes.


###v1.17
- Fixed some bugs.
- Added new weapons: Crystal Heal (need 6 satchel, when press `R` key), Turrel (`mouse 2` on RPG).
- Automatic parrent `CMD` by server to all clients: `rate 25000`, `cl_lw 0`, `cl_updaterate 20`, `cl_cmd 20`.
- Now fixed towers (as crossfire or doublecross).
- Gibs removed (for slow internet is great).