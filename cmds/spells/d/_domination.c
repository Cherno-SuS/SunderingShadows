//      Domination
#include <daemons.h>
#include <std.h>
#include <spell.h>
#include <magic.h>
#include <rooms.h>

inherit SPELL;

int duration, change, quitting;
object clothes, remote;

void create() {
    ::create();
    set_spell_name("domination");
    set_spell_level(([ "mage" : 5, "psion" : 8 ]));
    set_discipline("telepath");
    set_spell_sphere("enchantment_charm");
    set_syntax("cast CLASS domination on TARGET");
    set_description("By casting domination successfully, you will gain control over a chosen target. You can order the victim to do whatever is within his/her capability. For example, if the caster uses <cast domination on targetname>, targetname is the victim of the spell. Next, the mage can use <make targetname do kill kobold> or <make targetname emote kisses your feet>. The spell can be ended with <free targetname>.

%^BOLD%^%^RED%^N.B.%^RESET%^ If used on players this spell provide you only with limited subset of allowed commands.");
    mental_spell();
    set_verbal_comp();
    set_somatic_comp();
    set_target_required(1);
    set_save("will");
}


int query_domination_duration(object targ)
{
    int duration;
    duration = 60 + clevel * 60;
    duration = duration > 360 ? 360 : duration;
    return duration;
}

string query_cast_string()
{
    return caster->QCN + " glides " +
        caster->QP + " hand from side to side while " +
        "chanting hypnotically.";
}

int cant_be_dominated(object targ)
{
    return do_save(targ, 0) ||
        targ->query_property("no dominate") ||
        present("clothesx999", targ) ||
        mind_immunity_damage(targ, "default");
}

void spell_effect(int prof) {

    change = -2;

    if (!interactive(caster) || caster==target) {
        if(objectp(TO)) TO->remove();
        return;
    }

    if (!objectp(target)) {
        if(objectp(TO)) TO->remove();
        return;
    }

    if (!present(target,environment(caster))) {
        tell_object(caster,"%^BOLD%^Your target is not in this area.\n");
        if(objectp(TO)) TO->remove();
        return;
    }
    if (!caster->ok_to_kill(target)) {
        if(objectp(TO)) TO->remove();
        return;
    }

    if (cant_be_dominated(target)) {
        tell_room(environment(target), "%^RED%^Outraged at " + caster->QCN + " for " + caster->QP + " attempt at mind control, " + target->QCN + " attacks " + caster->QO + "!", ({ target, caster }));
        tell_object(target, "%^RED%^Outraged at " + caster->QCN + " for " + caster->QP + " attempt at mind control, you attack " + caster->QO + "!");
        tell_object(caster, "%^RED%^" + target->QCN + " attacks you, outraged at you for your attempt at mind control!");
        spell_successful();
        dest_effect();
        return;
    }

    duration = query_domination_duration(target);

    tell_object(caster,"%^GREEN%^You break into "+target->QCN+"'s mind and "
        "overcome "+target->QP+" willpower!");
    spell_successful();
    addSpellToCaster();
    change=0;
    clothes=new("/d/magic/obj/sclothes.c");
    clothes->set_property("spelled", ({TO}) );
    clothes->set_master(caster);
    clothes->set_spell(TO);
    clothes->move_is_ok(1);
    clothes->move(target);
    clothes->move_is_ok(0);
    remote=new("/d/magic/obj/remote.c");
    remote->set_property("spelled", ({TO}) );
    remote->set_caster(caster);
    remote->set_target(target);
    remote->set_spell(TO);
    remote->move_is_ok(1);
    remote->move(caster);
    remote->move_is_ok(0);
    call_out("dest_effect", duration);
}

void dest_effect() {
    if (find_call_out("check") != -1)
        remove_call_out("check");
    if (!clothes)
        if(objectp(TO)) TO->remove();
    if (clothes) {
        clothes->move_is_ok(1);
        clothes->move(ROOM_VOID);
        clothes->remove();
    }
    if (remote) {
        remote->move_is_ok(1);
        remote->move(ROOM_VOID);
        remote->remove();
    }
    if (target) {
        tell_room(environment(target),"%^YELLOW%^"+target->QCN+" blinks a couple times, then looks around.", target );
        tell_object(target,"%^YELLOW%^You blink a couple times, then look around.");
        if (quitting)
            target->force_me("quit");
    }
    if (!caster)
        destruct(TO);
    ::dest_effect();
    if(objectp(TO)) TO->remove();

}

void set_quitting() {
    quitting=1;
}
