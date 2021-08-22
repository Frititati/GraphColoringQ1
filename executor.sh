#/bin/bash

rm Jones/testv2
rm Jones/testv3
rm Jones/testv5
rm Ldf/test

cat report.log >> report_old.log
rm report.log

g++ -pthread Jones/multithreaded_v2.cpp -o Jones/testv2
g++ -pthread Jones/multithreaded_v3.cpp -o Jones/testv3
g++ -pthread Jones/multithreaded_v5.cpp -o Jones/testv5
g++ -pthread Ldf/ldf_multithreaded.cpp -o Ldf/test

number_threads=(2 4 8 16)
repetitions=(1 2 3 4 5)
# number_threads=(16)
# repetitions=(1)

workers_undirected=('Jones/testv2' 'Jones/testv3' 'Jones/testv5' 'Ldf/test')
workers_directed=('Jones/testv3' 'Ldf/test')

# workers_undirected=('Jones/testv2' 'Jones/testv3' 'Jones/testv5' 'Ldf/test')
# workers_directed=('Jones/testv3')



graphs_undirected=('rgg_n_2_15_s0.graph' 'rgg_n_2_21_s0.graph' 'rgg_n_2_24_s0.graph')
graphs_directed=('agrocyc_dag_uniq.gra' 'amaze_dag_uniq.gra' 'anthra_dag_uniq.gra'  'arXiv_sub_6000-1.gra' 'ba10k2d.gra' 'ba10k5d.gra' 'cit-Patents.scc.gra' 'citeseer.scc.gra' 'citeseer_sub_10720.gra' 'citeseer_sub_10720.gra' 'citeseerx.gra' 'cuda.gra' 'ecoo_dag_uniq.gra' 'go_sub_6793.gra' 'go_uniprot.gra' 'human_dag_uniq.gra' 'kegg_dag_uniq.gra' 'mtbrv_dag_uniq.gra' 'nasa_dag_uniq.gra' 'pubmed_sub_9000-1.gra' 'uniprotenc_100m.scc.gra' 'uniprotenc_150m.scc.gra' 'uniprotenc_22m.scc.gra' 'v10.gra' 'v100.gra' 'v1000.gra' 'vchocyc_dag_uniq.gra' 'xmark_dag_uniq.gra' 'yago_sub_6642.gra')

for w in ${workers_undirected[@]}; do
	for g in ${graphs_undirected[@]}; do
		for t in ${number_threads[@]}; do
			for r in ${repetitions[@]}; do
				echo "program:" $w "  graph(undir):" $g "  threads:" $t "  round:" $r
				echo "program:" $w "  graph(undir):" $g "  threads:" $t "  round:" $r >> report.log
				./$w Graphs/$g $t a.out >> report.log
				echo ""
				echo "" >> report.log
			done
		done
	done
done

for w in ${workers_directed[@]}; do
	for g in ${graphs_directed[@]}; do
		for t in ${number_threads[@]}; do
			for r in ${repetitions[@]}; do
				echo "program:" $w "  graph(dir):" $g "  threads:" $t "  round:" $r
				echo "program:" $w "  graph(dir):" $g "  threads:" $t "  round:" $r >> report.log
				./$w Graphs/$g $t a.out >> report.log
				echo ""
				echo "" >> report.log
			done
		done
	done
done