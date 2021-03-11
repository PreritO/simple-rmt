rm *.csv
if [ "$#" -ne 4 ]; then
    echo "Invalid Args passed"
    echo "Usage: $0 <PFP debugger: [none,debug]> <application: [pfp, nat, nat-debug, router, ecmp, firewall, paxos, congestion]> <verbosity level: [normal, minimal, p4profile, profile, debugger, debug]> <gdb debug: [none, gdb]>"
    exit
fi
debug_level="$1"
application="$2"
vlvl="$3"
gdb_level="$4"

if [[ $debug_level == "debug" ]] ; then
    if [[ $application == "pfp" ]] ; then 
        pfpdb rmt-sim --args "-Xp4 ../../../apps/pfp_baseline/simple_router.json -Xtpop ../../../apps/pfp_baseline/table.txt -v "$vlvl" -Xin ../../../apps/pfp_baseline/input.pcap -Xvalidation-out output.pcap" -v
    elif [[ $application == "nat" ]]; then
        pfpdb rmt-sim --args "-Xp4 ../../../apps/nat/nat.json -Xtpop ../../../apps/nat/nat_table.txt -v "$vlvl" -Xin ../../../apps/pcaps/200flows.pcap -Xvalidation-out output.pcap" -v
    elif [[ $application == "ecmp" ]]; then
        pfpdb rmt-sim --args "-Xp4 ../../../apps/ecmp/ecmp.json -Xtpop ../../../apps/ecmp/ecmp_table.txt -v "$vlvl" -Xin ../../../apps/pcaps/200flows.pcap -Xvalidation-out output.pcap" -v
    elif [[ $application == "firewall" ]]; then
        pfpdb rmt-sim --args "-Xp4 ../../../apps/stateful-firewall/firewall.json -Xtpop ../../../apps/stateful-firewall/firewall_table.txt -v "$vlvl" -Xin ../../../apps/pcaps/400pkts.pcap -Xvalidation-out output.pcap" -v
    elif [[ $application == "congestion" ]]; then
        pfpdb rmt-sim --args "-Xp4 ../../../apps/congestion-control/ipv4-forward.json -Xtpop ../../../apps/congestion-control/ipv4_table.txt -v "$vlvl" -Xin ../../../apps/pcaps/400pkts.pcap -Xvalidation-out output.pcap" -v
    elif [[ $application == "nat-debug" ]]; then
        pfpdb rmt-sim --args "-Xp4 ../../../apps/nat-debug/nat.json -Xtpop ../../../apps/nat-debug/nat_table_debug.txt -v "$vlvl" -Xin ../../../apps/pcaps/200flows.pcap -Xvalidation-out output.pcap" -v
    else
        echo ""$application" not yet implemented.."
        exit
    fi
else
    if [[ $application == "pfp" ]] ; then
        if [[ $gdb_level == "gdb" ]] ; then
            gdb --args ./rmt-sim -c Configs/ -Xp4 ../../../apps/pfp_baseline/simple_router.json -Xtpop ../../../apps/pfp_baseline/table.txt -Xin ../../../apps/pfp_baseline/input.pcap -Xvalidation-out reordered-output.pcap -v "$vlvl"
        else
            ./rmt-sim -c Configs/ -Xp4 ../../../apps/pfp_baseline/simple_router.json -Xtpop ../../../apps/pfp_baseline/table.txt -Xin ../../../apps/pfp_baseline/input.pcap -Xvalidation-out reordered-output.pcap -v "$vlvl"
        fi
    elif [[ $application == "nat" ]]; then
        if [[ $gdb_level == "gdb" ]] ; then
            gdb --args ./rmt-sim -c Configs/ -Xp4 ../../../apps/nat/nat.json -Xtpop ../../../apps/nat/nat_table.txt -Xin ../../../apps/pcaps/200flows.pcap -Xvalidation-out reordered-output.pcap -v "$vlvl"
        else
            ./rmt-sim -c Configs/ -Xp4 ../../../apps/nat/nat.json -Xtpop ../../../apps/nat/nat_table.txt -Xin ../../../apps/pcaps/200flows.pcap -Xvalidation-out reordered-output.pcap -v "$vlvl"
        fi
    elif [[ $application == "congestion" ]]; then
        if [[ $gdb_level == "gdb" ]] ; then
            gdb --args ./rmt-sim -c Configs/ -Xp4 ../../../apps/congestion-control/ipv4-forward.json -Xtpop ../../../apps/congestion-control/ipv4_table.txt -Xin ../../../apps/pcaps/200flows.pcap -Xvalidation-out reordered-output.pcap -v "$vlvl"
        else
            ./rmt-sim -c Configs/ -Xp4 ../../../apps/congestion-control/ipv4-forward.json -Xtpop ../../../apps/congestion-control/ipv4_table.txt -Xin ../../../apps/pcaps/200flows.pcap -Xvalidation-out reordered-output.pcap -v "$vlvl"
        fi
    elif [[ $application == "ecmp" ]]; then
        if [[ $gdb_level == "gdb" ]] ; then
            gdb --args ./rmt-sim -c Configs/ -Xp4 ../../../apps/ecmp/ecmp.json -Xtpop ../../../apps/ecmp/ecmp_table.txt -Xin ../../../apps/pcaps/200flows.pcap -Xvalidation-out reordered-output.pcap -v "$vlvl"
        else
            ./rmt-sim -c Configs/ -Xp4 ../../../apps/ecmp/ecmp.json -Xtpop ../../../apps/ecmp/ecmp_table.txt -Xin ../../../apps/pcaps/200flows.pcap -Xvalidation-out reordered-output.pcap -v "$vlvl"
        fi
    elif [[ $application == "firewall" ]]; then
        if [[ $gdb_level == "gdb" ]] ; then
            gdb --args ./rmt-sim -c Configs/ -Xp4 ../../../apps/stateful-firewall/firewall.json -Xtpop ../../../apps/stateful-firewall/firewall_table.txt -Xin ../../../apps/pcaps/400pkts.pcap -Xvalidation-out reordered-output.pcap -v "$vlvl"
        else
            ./rmt-sim -c Configs/ -Xp4 ../../../apps/stateful-firewall/firewall.json -Xtpop ../../../apps/stateful-firewall/firewall_table.txt -Xin ../../../apps/pcaps/400pkts.pcap -Xvalidation-out reordered-output.pcap -v "$vlvl"
        fi
    elif [[ $application == "nat-debug" ]]; then
        if [[ $gdb_level == "gdb" ]] ; then
            gdb --args ./rmt-sim -c Configs/ -Xp4 ../../../apps/nat-debug/nat.json -Xtpop ../../../apps/nat-debug/nat_table_debug.txt -Xin ../../../apps/pcaps/200flows.pcap -Xvalidation-out reordered-output.pcap -v "$vlvl"
        else
            ./rmt-sim -c Configs/ -Xp4 ../../../apps/nat-debug/nat.json -Xtpop ../../../apps/nat-debug/nat_table_debug.txt -Xin ../../../apps/pcaps/200flows.pcap -Xvalidation-out reordered-output.pcap -v "$vlvl"
        fi
    else 
        echo ""$application" not yet implemented.."
        exit
    fi
fi

#sample call: ./runme.sh none nat normal none