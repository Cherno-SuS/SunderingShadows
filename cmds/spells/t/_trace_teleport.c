#include <spell.h>
inherit SPELL;

#define TRACEOBJ "/d/magic/obj/trace_teleport"

void create()
{
    ::create();
    set_spell_name("trace teleport");
    set_spell_level(([ "mage" : 5, "psion" : 4 ]));
    set_spell_sphere("divination");
    set_domains("mentalism");
    set_syntax("cast CLASS trace teleport");
    set_description("This spell allows an arcanist to trace any recent teleportation from or out of the area and memorize the source or destination. This spell won't work on portal-like sources of travel.");
    set_verbal_comp();
    set_somatic_comp();
}

string query_cast_string()
{
    return "%^BOLD%^%^WHITE%^" + caster->QCN + " closes the eyes chanting.%^RESET%^";
}

void spell_effect()
{
    object trace, obj;
    string fname;

    spell_successful();
    if (!objectp(trace = place->query_property("recent teleport"))) {
        tell_object(caster,"%^BOLD%^%^WHITE%^You fail to find any teleportation traces.");
        dest_effect();
        return;
    }
    tell_object(caster,"%^BOLD%^%^RED%^You sense there was a recent teleport from or into%^RESET%^ " + trace->query_short());
    obj = new(TRACEOBJ);
    obj->setup_mem(file_name(trace), caster);
    obj->move(caster);
    dest_effect();
}
