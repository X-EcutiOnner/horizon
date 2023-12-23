---------------------------------------------------------------------------
--       _   _            _                        --
--      | | | |          (_)                       --
--      | |_| | ___  _ __ _ _______  _ __          --
--      |  _  |/ _ \| '__| |_  / _ \| '_  \        --
--      | | | | (_) | |  | |/ / (_) | | | |        --
--      \_| |_/\___/|_|  |_/___\___/|_| |_|        --
---------------------------------------------------------------------------
-- This file is part of Horizon (c).
-- Copyright (c) 2018 Horizon Dev Team.
--
-- Base Author - Sagun K. (sagunxp@gmail.com)
--
-- This library is free software; you can redistribute it and/or modify
-- it under the terms of the GNU General Public License as published by
-- the Free Software Foundation, either version 3 of the License, or
-- (at your option) any later version.
--
-- This library is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU General Public License for more details.
--
-- You should have received a copy of the GNU General Public License
-- along with this library.  If not, see <http://www.gnu.org/licenses/>.
---------------------------------------------------------------------------
--- Include NPC Definitions File.
---------------------------------------------------------------------------

scripts = {
	-- Warps
	"npcs/warps/pvp.lua",
	"npcs/warps/guildcastles.lua",
	-- Warps: Cities
	"npcs/warps/cities/ayothaya.lua",
	"npcs/warps/cities/alberta.lua",
	"npcs/warps/cities/aldebaran.lua",
	"npcs/warps/cities/amatsu.lua",
	"npcs/warps/cities/comodo.lua",
	"npcs/warps/cities/einbech.lua",
	"npcs/warps/cities/einbroch.lua",
	"npcs/warps/cities/geffen.lua",
	"npcs/warps/cities/gonryun.lua",
	"npcs/warps/cities/hugel.lua",
	"npcs/warps/cities/lighthalzen.lua",
	"npcs/warps/cities/louyang.lua",
	"npcs/warps/cities/lutie.lua",
	"npcs/warps/cities/manuk.lua",
	"npcs/warps/cities/mid_camp.lua",
	"npcs/warps/cities/morroc.lua",
	"npcs/warps/cities/moscovia.lua",
	"npcs/warps/cities/nameless.lua",
	"npcs/warps/cities/niflheim.lua",
	"npcs/warps/cities/payon.lua",
	"npcs/warps/cities/prontera.lua",
	"npcs/warps/cities/splendide.lua",
	"npcs/warps/cities/umbala.lua",
	"npcs/warps/cities/veins.lua",
	"npcs/warps/cities/yuno.lua",
	-- Warps: Dungeons
	"npcs/warps/dungeons/ama_dun.lua",
	"npcs/warps/dungeons/abbey.lua",
	"npcs/warps/dungeons/abyss.lua",
	"npcs/warps/dungeons/alde_dun.lua",
	"npcs/warps/dungeons/anthell.lua",
	"npcs/warps/dungeons/ayo_dun.lua",
	"npcs/warps/dungeons/beach_dun.lua",
	"npcs/warps/dungeons/c_tower.lua",
	"npcs/warps/dungeons/ein_dun.lua",
	"npcs/warps/dungeons/gef_dun.lua",
	"npcs/warps/dungeons/gon_dun.lua",
	"npcs/warps/dungeons/ice_dun.lua",
	"npcs/warps/dungeons/in_sphinx.lua",
	"npcs/warps/dungeons/iz_dun.lua",
	"npcs/warps/dungeons/juperos.lua",
	"npcs/warps/dungeons/kh_dun.lua",
	"npcs/warps/dungeons/lhz_dun.lua",
	"npcs/warps/dungeons/lou_dun.lua",
	"npcs/warps/dungeons/mag_dun.lua",
	"npcs/warps/dungeons/mjo_dun.lua",
	"npcs/warps/dungeons/moc_pryd.lua",
	"npcs/warps/dungeons/mosk_dun.lua",
	"npcs/warps/dungeons/odin.lua",
	"npcs/warps/dungeons/orcsdun.lua",
	"npcs/warps/dungeons/pay_dun.lua",
	"npcs/warps/dungeons/prt_maze.lua",
	"npcs/warps/dungeons/ra_san.lua",
	"npcs/warps/dungeons/tha_t.lua",
	"npcs/warps/dungeons/thor_v.lua",
	"npcs/warps/dungeons/treasure.lua",
	"npcs/warps/dungeons/tur_dun.lua",
	"npcs/warps/dungeons/um_dun.lua",
	"npcs/warps/dungeons/xmas_dun.lua",
	-- Warps: Fields
	"npcs/warps/fields/glastheim.lua",
	"npcs/warps/fields/amatsu_fild.lua",
	"npcs/warps/fields/ein_fild.lua",
	"npcs/warps/fields/gefenia.lua",
	"npcs/warps/fields/jawaii.lua",
	"npcs/warps/fields/lhalzen_fild.lua",
	"npcs/warps/fields/lutie_fild.lua",
	"npcs/warps/fields/man_fild.lua",
	"npcs/warps/fields/mtmjolnir.lua",
	"npcs/warps/fields/spl_fild.lua",
	"npcs/warps/fields/umbala_fild.lua",
	-- Warps: Other
	"npcs/warps/other/kiel.lua",
	"npcs/warps/other/god.lua",
	"npcs/warps/other/jobquests.lua",
	"npcs/warps/other/arena.lua",
	"npcs/warps/other/airplane.lua",
	-- Warps: Renewal
	"npcs/re/warps/guildcastles.lua",
	-- Warps: Renewal: Cities
	"npcs/re/warps/cities/yggdrasil.lua",
	"npcs/re/warps/cities/rachel.lua",
	"npcs/re/warps/cities/malaya.lua",
	"npcs/re/warps/cities/malangdo.lua",
	"npcs/re/warps/cities/izlude.lua",
	"npcs/re/warps/cities/eclage.lua",
	"npcs/re/warps/cities/dicastes.lua",
	"npcs/re/warps/cities/dewata.lua",
	"npcs/re/warps/cities/brasilis.lua",
	"npcs/re/warps/cities/prontera.lua",
	-- Warps: Renewal: Dungeons
	"npcs/re/warps/dungeons/moc_pryd.lua",
	"npcs/re/warps/dungeons/ecl_dun.lua",
	"npcs/re/warps/dungeons/dic_dun.lua",
	"npcs/re/warps/dungeons/bra_dun.lua",
	-- Warps: Renewal: Fields
	"npcs/re/warps/fields/yuno_fild.lua",
	"npcs/re/warps/fields/veins_fild.lua",
	"npcs/re/warps/fields/rachel_fild.lua",
	"npcs/re/warps/fields/payon_fild.lua",
	"npcs/re/warps/fields/prontera_fild.lua",
	"npcs/re/warps/fields/hugel_fild.lua",
	"npcs/re/warps/fields/morroc_fild.lua",
	"npcs/re/warps/fields/dic_fild.lua",
	"npcs/re/warps/fields/geffen_fild.lua",
	"npcs/re/warps/fields/com_fild.lua",
	"npcs/re/warps/fields/bra_fild.lua",
	"npcs/re/warps/fields/bif_fild.lua",
	-- Warps: Renewal: Other
	"npcs/re/warps/other/sign.lua",
	"npcs/re/warps/other/s_workshop.lua",
	"npcs/re/warps/other/jobquests.lua",
	"npcs/re/warps/other/dimensional_gap.lua",
	"npcs/re/warps/other/arena.lua",

	-- NPC
	"npcs/npcs.lua",

	-- Monster
	"monsters/dungeons/gefenia.lua",
	"monsters/dungeons/lou_dun.lua",
	"monsters/dungeons/mag_dun.lua",
	"monsters/dungeons/kh_dun.lua",
	"monsters/dungeons/ein_dun.lua",
	"monsters/dungeons/moc_pryd.lua",
	"monsters/dungeons/bra_dun.lua",
	"monsters/dungeons/mjo_dun.lua",
	"monsters/dungeons/prt_maze.lua",
	"monsters/dungeons/mal_dun.lua",
	"monsters/dungeons/ice_dun.lua",
	"monsters/dungeons/orcsdun.lua",
	"monsters/dungeons/ma_dun.lua",
	"monsters/dungeons/tur_dun.lua",
	"monsters/dungeons/tha_t.lua",
	"monsters/dungeons/ayo_dun.lua",
	"monsters/dungeons/iz_dun.lua",
	"monsters/dungeons/xmas_dun.lua",
	"monsters/dungeons/gld_dunSE.lua",
	"monsters/dungeons/dic_dun.lua",
	"monsters/dungeons/yggdrasil.lua",
	"monsters/dungeons/dew_dun.lua",
	"monsters/dungeons/gef_dun.lua",
	"monsters/dungeons/alde_dun.lua",
	"monsters/dungeons/beach_dun.lua",
	"monsters/dungeons/ra_san.lua",
	"monsters/dungeons/odin.lua",
	"monsters/dungeons/gld_re.lua",
	"monsters/dungeons/abbey.lua",
	"monsters/dungeons/ecl_tdun.lua",
	"monsters/dungeons/pay_dun.lua",
	"monsters/dungeons/thor_v.lua",
	"monsters/dungeons/lhz_dun.lua",
	"monsters/dungeons/nyd_dun.lua",
	"monsters/dungeons/ama_dun.lua",
	"monsters/dungeons/anthell.lua",
	"monsters/dungeons/treasure.lua",
	"monsters/dungeons/juperos.lua",
	"monsters/dungeons/mosk_dun.lua",
	"monsters/dungeons/prt_sew.lua",
	"monsters/dungeons/glastheim.lua",
	"monsters/dungeons/c_tower.lua",
	"monsters/dungeons/abyss.lua",
	"monsters/dungeons/in_sphinx.lua",
	"monsters/dungeons/gon_dun.lua",

	"monsters/fields/malaya.lua",
	"monsters/fields/splendide.lua",
	"monsters/fields/rachel.lua",
	"monsters/fields/moscovia.lua",
	"monsters/fields/veins.lua",
	"monsters/fields/comodo.lua",
	"monsters/fields/yuno.lua",
	"monsters/fields/ayothaya.lua",
	"monsters/fields/louyang.lua",
	"monsters/fields/hugel.lua",
	"monsters/fields/mjolnir.lua",
	"monsters/fields/gonryun.lua",
	"monsters/fields/dicastes.lua",
	"monsters/fields/brasilis.lua",
	"monsters/fields/morocc.lua",
	"monsters/fields/lutie.lua",
	"monsters/fields/einbroch.lua",
	"monsters/fields/eclage.lua",
	"monsters/fields/umbala.lua",
	"monsters/fields/bifrost.lua",
	"monsters/fields/lighthalzen.lua",
	"monsters/fields/prontera.lua",
	"monsters/fields/geffen.lua",
	"monsters/fields/dewata.lua",
	"monsters/fields/niflheim.lua",
	"monsters/fields/amatsu.lua",
	"monsters/fields/manuk.lua",
	"monsters/fields/payon.lua",

	"monsters/citycleaners.lua",
	"monsters/champion.lua",
	"monsters/towns.lua",
	"monsters/int_land.lua",
}
