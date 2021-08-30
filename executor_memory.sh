#/bin/bash

# rm Jones/test_multi_2
# rm Jones/test_multi_3
# rm Jones/test_multi_5
# rm Ldf/test_multi_1

# rm Jones/test_seq_1
# rm Jones/test_seq_2
# rm Jones/test_seq_2_spinoff
# rm Ldf/test_seq_1

# cat report.log >> report_old.log
# rm report.log

# g++ -pthread Jones/multithreaded_v2.cpp -o Jones/test_multi_2
# g++ -pthread Jones/multithreaded_v3.cpp -o Jones/test_multi_3
# g++ -pthread Jones/multithreaded_v5.cpp -o Jones/test_multi_5
# g++ -pthread Ldf/ldf_multithreaded.cpp -o Ldf/test_multi_1

# g++ Jones/sequential_first_attempt.cpp -o Jones/test_seq_1
# g++ Jones/sequential_second_attempt.cpp -o Jones/test_seq_2
# g++ Jones/sequential_second_spinoff_attempt.cpp -o Jones/test_seq_2_spinoff
# g++ Ldf/sequential_first_attempt.cpp -o Ldf/test_seq_1

number_threads=(2 4 8 16)
# number_threads=(16)
# repetitions=(1)

workers_undirected=('Jones/test_multi_2' 'Jones/test_multi_3' 'Jones/test_multi_5' 'Ldf/test_multi_1')
workers_directed=('Jones/test_multi_3' 'Ldf/test_multi_1')

workers_sequential=('Jones/test_seq_1' 'Jones/test_seq_2' 'Jones/test_seq_2_spinoff' 'Ldf/test_seq_1')

# workers_undirected=('Jones/testv2' 'Jones/testv3' 'Jones/testv5' 'Ldf/test')
# workers_directed=('Jones/testv3')



graphs_undirected=('rgg_n_2_15_s0.graph' 'rgg_n_2_21_s0.graph' 'rgg_n_2_24_s0.graph')
graphs_directed=('agrocyc_dag_uniq.gra' 'amaze_dag_uniq.gra' 'anthra_dag_uniq.gra'  'arXiv_sub_6000-1.gra' 'ba10k2d.gra' 'ba10k5d.gra' 'cit-Patents.scc.gra' 'citeseer.scc.gra' 'citeseer_sub_10720.gra' 'citeseerx.gra' 'cuda.gra' 'ecoo_dag_uniq.gra' 'go_sub_6793.gra' 'go_uniprot.gra' 'human_dag_uniq.gra' 'kegg_dag_uniq.gra' 'mtbrv_dag_uniq.gra' 'nasa_dag_uniq.gra' 'pubmed_sub_9000-1.gra' 'uniprotenc_100m.scc.gra' 'uniprotenc_150m.scc.gra' 'uniprotenc_22m.scc.gra' 'v10.gra' 'v100.gra' 'v1000.gra' 'vchocyc_dag_uniq.gra' 'xmark_dag_uniq.gra' 'yago_sub_6642.gra')

graphs_sequential=('agrocyc_dag_uniq.gra' 'amaze_dag_uniq.gra' 'anthra_dag_uniq.gra'  'arXiv_sub_6000-1.gra' 'ba10k2d.gra' 'ba10k5d.gra' 'cit-Patents.scc.gra' 'citeseer.scc.gra' 'citeseer_sub_10720.gra' 'citeseerx.gra' 'cuda.gra' 'ecoo_dag_uniq.gra' 'go_sub_6793.gra' 'go_uniprot.gra' 'human_dag_uniq.gra' 'kegg_dag_uniq.gra' 'mtbrv_dag_uniq.gra' 'nasa_dag_uniq.gra' 'pubmed_sub_9000-1.gra' 'uniprotenc_100m.scc.gra' 'uniprotenc_150m.scc.gra' 'uniprotenc_22m.scc.gra' 'v10.gra' 'v100.gra' 'v1000.gra' 'vchocyc_dag_uniq.gra' 'xmark_dag_uniq.gra' 'yago_sub_6642.gra' 'rgg_n_2_15_s0.graph' 'rgg_n_2_21_s0.graph' 'rgg_n_2_24_s0.graph')

for w in ${workers_undirected[@]}; do
	for g in ${graphs_undirected[@]}; do
		echo "program:" $w "  graph(undir):" $g
		# echo "program:" $w "  graph(undir):" $g >> report.log
		runlim -o "memory/${w}/${g}" ./$w Graphs/$g 16
		echo ""
		# echo "" >> report.log
	done
done

for w in ${workers_directed[@]}; do
	for g in ${graphs_directed[@]}; do
		echo "program:" $w "  graph(dir):" $g
		# echo "program:" $w "  graph(dir):" $g >> report.log
		runlim -o "memory/${w}/${g}" ./$w Graphs/$g 16
		echo ""
		# echo "" >> report.log
	done
done

for w in ${workers_sequential[@]}; do
	for g in ${graphs_sequential[@]}; do
		echo "program:" $w "  graph:" $g
		# echo "program:" $w "  graph:" $g >> report.log
		runlim -o "memory/${w}/${g}" ./$w Graphs/$g
		echo ""
		# echo "" >> report.log
	done
done