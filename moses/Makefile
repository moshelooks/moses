CC = g++

PROJ_NAME = moses

LINK_FLAGS = -Wall -Iutils/ -Itrees/ -Irewrite -I./ -Imodeling/ -Ifitness/ \
             -Ialignment/ -Isim/ -Ilocal/ -O3
COMP_FLAGS = -Wall -Wno-sign-compare -Iutils/ -Itrees/ -Irewrite -I./ \
             -Imodeling/ -Ifitness/ -Ialignment/ -Isim/ -Ilocal/ -O3

src := $(wildcard *.cc) $(wildcard utils/*.cc) $(wildcard trees/*.cc) $(wildcard modeling/*.cc) $(wildcard fitness/*.cc) $(wildcard alignment/*.cc) $(wildcard main/*.cc) $(wildcard rewrite/*.cc) $(wildcard sim/*.cc) $(wildcard local/*.cc)

obj := $(patsubst %.cc,%.o,$(src))

all: $(PROJ_NAME)

%.o: %.cc
	$(CC) $(COMP_FLAGS) $< -c -o $@

$(PROJ_NAME): $(obj)
	$(CC) $(LINK_FLAGS) $^ -o $(PROJ_NAME)
run:
	$(PROJ_NAME)
clean:
	find -regex ".*~\|.*\.o"|xargs rm -f
	rm -f $(PROJ_NAME)
	rm -f $(PROJ_NAME).exe*

depend:
	makedepend -Y -- $(COMP_FLAGS) -- $(src)

# DO NOT DELETE

utils/exceptions.o: utils/exceptions.h utils/utils.h
utils/io_util.o: utils/io_util.h utils/tree.h utils/basic_types.h
utils/math_util.o: utils/math_util.h utils/selection.h utils/func_util.h
utils/MedianStore.o: utils/MedianStore.h
utils/selection.o: utils/selection.h utils/math_util.h utils/func_util.h
utils/utils.o: utils/utils.h utils/exceptions.h
modeling/BB.o: modeling/BB.h modeling/modeling.h
modeling/BDe.o: modeling/BDe.h utils/func_util.h utils/math_util.h
modeling/BDe.o: utils/MedianStore.h
modeling/build_tree.o: modeling/build_tree.h alignment/ProtoVertex.h
modeling/build_tree.o: utils/Vertex.h utils/tree.h utils/id.h
modeling/build_tree.o: utils/basic_types.h utils/set_util.h
modeling/build_tree.o: modeling/modeling.h
modeling/dtree.o: modeling/dtree.h modeling/modeling.h utils/tree.h
modeling/dtree.o: utils/basic_types.h utils/math_util.h
modeling/dtree_learn.o: modeling/dtree_learn.h modeling/dtree_modeling.h
modeling/dtree_learn.o: modeling/dtree.h modeling/modeling.h utils/tree.h
modeling/dtree_learn.o: utils/basic_types.h utils/math_util.h
modeling/dtree_learn.o: modeling/Dependance.h modeling/BDe.h
modeling/dtree_learn.o: utils/func_util.h utils/MedianStore.h
modeling/make_empty_model.o: modeling/make_empty_model.h
modeling/make_empty_model.o: modeling/dtree_modeling.h modeling/dtree.h
modeling/make_empty_model.o: modeling/modeling.h utils/tree.h
modeling/make_empty_model.o: utils/basic_types.h utils/math_util.h
modeling/make_empty_model.o: modeling/Dependance.h alignment/ProtoVertex.h
modeling/make_empty_model.o: utils/Vertex.h utils/id.h utils/set_util.h
alignment/ProtoVertex.o: alignment/ProtoVertex.h utils/Vertex.h utils/tree.h
alignment/ProtoVertex.o: utils/id.h utils/basic_types.h utils/set_util.h
alignment/ProtoVertex.o: utils/func_util.h
alignment/scoring.o: alignment/scoring.h utils/tree.h utils/basic_types.h
alignment/scoring.o: utils/math_util.h utils/hash_util.h utils/func_util.h
alignment/scoring.o: utils/selection.h alignment/ProtoVertex.h utils/Vertex.h
alignment/scoring.o: utils/id.h utils/set_util.h modeling/dtree_modeling.h
alignment/scoring.o: modeling/dtree.h modeling/modeling.h
alignment/scoring.o: modeling/Dependance.h
main/main.o: rewrite/simplify.h rewrite/rewrite.h utils/tree.h utils/id.h
main/main.o: utils/basic_types.h rewrite/rewrite_impl.h utils/math_util.h
main/main.o: trees/TreeComp.h trees/SubtreeOrder.h rewrite/ant_rewrite.h
main/main.o: rewrite/logical_rewrite.h rewrite/flat_normal_form.h
main/main.o: rewrite/logical_rewrite_impl.h rewrite/bool_if_rewrite.h
main/main.o: rewrite/enf.h utils/func_util.h utils/set_util.h
main/main.o: rewrite/algebraic_rewrite.h rewrite/algebraic_rewrite_impl.h
main/main.o: trees/tree_generation.h utils/hash_util.h
main/main.o: trees/semantic_sampler.h utils/io_util.h utils/selection.h
main/main.o: trees/uniform_tree_sampling.h trees/tree_util.h utils/Vertex.h
main/main.o: modeling/BDe.h utils/MedianStore.h fitness/boolean.h
main/main.o: utils/bool_util.h fitness/ant.h alignment/scoring.h
main/main.o: alignment/ProtoVertex.h modeling/build_tree.h
main/main.o: modeling/modeling.h local/lmodeling.h local/knobs.h
main/main.o: modeling/dtree_modeling.h modeling/dtree.h modeling/Dependance.h
main/main.o: local/moses.h modeling/dtree_learn.h fitness/join.h
main/main.o: modeling/BB.h
rewrite/enf.o: rewrite/enf.h utils/tree.h rewrite/logical_rewrite.h
rewrite/enf.o: rewrite/rewrite.h utils/id.h utils/basic_types.h
rewrite/enf.o: rewrite/rewrite_impl.h utils/math_util.h trees/TreeComp.h
rewrite/enf.o: trees/SubtreeOrder.h rewrite/flat_normal_form.h
rewrite/enf.o: rewrite/logical_rewrite_impl.h utils/func_util.h
rewrite/enf.o: utils/set_util.h
rewrite/flat_normal_form.o: rewrite/flat_normal_form.h trees/SubtreeOrder.h
rewrite/flat_normal_form.o: utils/id.h utils/basic_types.h utils/tree.h
rewrite/flat_normal_form.o: utils/math_util.h
local/knobs.o: local/knobs.h utils/tree.h utils/selection.h utils/math_util.h
local/knobs.o: utils/func_util.h utils/Vertex.h utils/id.h
local/knobs.o: utils/basic_types.h trees/tree_util.h
local/lmodeling.o: local/lmodeling.h local/knobs.h utils/tree.h
local/lmodeling.o: utils/selection.h utils/math_util.h utils/func_util.h
local/lmodeling.o: utils/Vertex.h utils/id.h utils/basic_types.h
local/lmodeling.o: trees/tree_util.h modeling/dtree_modeling.h
local/lmodeling.o: modeling/dtree.h modeling/modeling.h modeling/Dependance.h
local/lmodeling.o: trees/semantic_sampler.h utils/hash_util.h utils/io_util.h
local/lmodeling.o: trees/uniform_tree_sampling.h trees/TreeComp.h
local/lmodeling.o: trees/SubtreeOrder.h
local/moses.o: local/moses.h local/lmodeling.h local/knobs.h utils/tree.h
local/moses.o: utils/selection.h utils/math_util.h utils/func_util.h
local/moses.o: utils/Vertex.h utils/id.h utils/basic_types.h
local/moses.o: trees/tree_util.h modeling/dtree_modeling.h modeling/dtree.h
local/moses.o: modeling/modeling.h modeling/Dependance.h
local/moses.o: trees/semantic_sampler.h utils/hash_util.h utils/io_util.h
local/moses.o: trees/uniform_tree_sampling.h trees/TreeComp.h
local/moses.o: trees/SubtreeOrder.h modeling/dtree_learn.h utils/bool_util.h
local/moses.o: modeling/BDe.h utils/MedianStore.h
