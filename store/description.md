# WrathClassicAPI

Adds a set of convenience Lua functions to the 3.3.5a client, available to any
addon with no companion addon to install. The functions are registered directly
into the in-game Lua state as global functions and `C_*` namespaces.

Highlights:

- **Retail-like `/reload`** — the addon environment is re-read in place: newly
  added addon folders load, new files and `##` TOC directive edits take effect,
  and deleted addons drop off the list. No client restart needed.
- **Modern API backports** — `C_Item`, `C_Spell`, `C_UnitAuras`, `C_QuestLog`,
  `C_GossipInfo`, `C_Reputation`, `C_Timer`, `CombatLogGetCurrentEventInfo`,
  `Mixin`, and more, so addons written against later API versions run on 3.3.5a.
- **Backported events** — `BAG_UPDATE_DELAYED`, `QUEST_TURNED_IN` (with the XP
  and money actually granted), `QUEST_REMOVED`, `FACTION_STANDING_CHANGED`.
- **Engine improvements** — `GetItemInfo` on an uncached item now queries the
  server, so later calls return data and `GET_ITEM_INFO_RECEIVED` fires;
  `GameTooltip:SetSpellByID` works for any spell, not just spellbook entries.

It installs through WarcraftXL's hook registry, so it runs alongside other
extensions that detour the same engine functions.
