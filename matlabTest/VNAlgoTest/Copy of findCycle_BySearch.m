function SC = findCycle_BySearch(E, Adj_list, from)
% 通过搜索的方法计算单向回路

m = size(Adj_list,1);
dfn = zeros(1,m); % 已访问节点的顺序
color = zeros(1,m); % 节点是否已被访问过

parent(1:m)=-1;   % 记录节点的父节点均未知

k = 1; % 计数器
SC={};
for u=1:m
    if dfn(u)==0
        [SC, dfn, color, k, parent]=dfs(Adj_list, E, from, dfn, color, k, parent, SC);
    end
end

    function [outSC, outDfn, outColor, outK, outParent]=dfs(Adj_list, E, u, dfn, color, k, parent, SC)
        dfn(u)=k;
        k = k+1;
        color(u)=1;

        for e = Adj_list{u}
            w = E(e, 3);
            if color(w)
                % 找到一个单向回路
                nodesOfCycle = findNodesOfCycle(parent, u, w);
                edgesOfCycle = findEdgesOfCycle(E, Adj_list, nodesOfCycle);
                if ~isempty(edgesOfCycle)
                    SC{end+1} = edgesOfCycle;
                end
            elseif dfn(w)==0
                parent(w)=u;
                [outSC, outDfn, outColor, outK, outParent]=dfs(Adj_list, E, w, dfn, color, k, parent, SC);
                dfn=outDfn;
                color=outColor;
                k=outK;
                parent=outParent;
                SC=outSC;
            end
        end
        color(u)=0;

        outDfn=dfn;
        outColor=color;
        outK=k;
        outParent=parent;
        outSC=SC;
    end

    function nodesOfCycle = findNodesOfCycle(parent, u, v)
        % 查找单向回路u->v->u上的节点
        % 用节点表示单向回路
        nodesOfCycle =[];
        while u ~= v
            nodesOfCycle = [u nodesOfCycle]; % 逆向
            u = parent(u);
        end
        nodesOfCycle = [nodesOfCycle v];
    end

    function edgesOfCycle = findEdgesOfCycle(E, Adj_list, nodesOfCycle)
        % 根据节点单向回路查找分支单向回路
        % 用分支表示单向回路
        edgesOfCycle = [];
        if length(nodesOfCycle) <= 1 % 必须至少存在2个节点
            return;
        end

        % 求解单向回路的分支路径
        u = nodesOfCycle(1); % 取出第1个节点作为回路的开始点
        nodesOfCycle = [nodesOfCycle u]; % 构成一个环
        for v = nodesOfCycle
            if u == v
                continue;
            end

            for e = Adj_list{u}
                if E(e, 3) == v
                    edgesOfCycle = [edgesOfCycle e];
                end
            end
            u = v;
        end
    end

	% 原来的方法备份
	% 经过测试发现效果都是一样的，
	% 原来的方法和现在的dfs都一样不能找全所有的单向回路
 function [outSC, outColor, outParent] = dfs_visit(Adj_list, E, color, parent, u, SC)
        % 执行dfs搜索，查找树边分支
        color(u) = 1; % gray
        for e = Adj_list{u}
            v = E(e, 3); % 出边分支的末节点
            if color(v) == 0 % while--第一次访问
                parent(v) = u; % 记录父节点
                [outSC, outColor, outParent] = dfs_visit(Adj_list, E, color, parent, v, SC);
                color = outColor;
                parent = outParent;
                SC = outSC;
            else if color(v) == 1 % gray--已访问，但可能还有其它方向未探索
                    % 这时就表示存在一个单向回路u->v->u
                    % 搜索到的节点是按顺序排列的
                    nodesOfCycle = findNodesOfCycle(parent, u, v);
                    
                    edgesOfCycle = findEdgesOfCycle(E, Adj_list, nodesOfCycle);
                    if ~isempty(edgesOfCycle)                        
                        SC{end+1} = edgesOfCycle;
                    end
                end
            end
        end
        color(u)=2;
        outColor = color;
        outParent = parent;
        outSC = SC;
    end	

end
