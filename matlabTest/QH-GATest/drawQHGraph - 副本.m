function splitedEdges = drawQHGraph(A, V, E, from, P, toDraw)
% 绘制Q-H平衡图

theta = 10^-3; % 流量误差

% 从P中提取最小通路的最小流量
minWidth = cell2mat(P(1:end, 2)); % 提取P中记录的通路流量宽度
disp(sprintf('\n总的流量宽度之和：%.3f', sum(minWidth)));
widthEdges = cell2mat(P(1:end, 3));

% 打印独立通路的宽度分支以及宽度流量
len = length(minWidth);
msg = [];
for i=1:len
    msg = [msg sprintf('第%d条通路：%d -- %.3f\n', i, E(widthEdges(i), 1),  minWidth(i))];
end
disp(msg);

% 尝试计算虚拟分支平衡阻力
balanceVirtualEdgesH(V, E, P);

% 开始绘制Q-H平衡图
EdgeRectPos = initEdgeRectPos(E);   % 初始化分支矩形块坐标
H = zeros(1, length(V));% 初始化所有节点的Y坐标为0(压能)
% H = caclNodeHeight(V, E, from, P);           % 计算节点压能
splitedEdges = []; % 记录被分割的分支

if toDraw
    % 绘制Q-H平衡图的准备工作(设置图的属性，例如x、y轴以及标题)
    setQHGraphDraw(sum(minWidth), caclMaxNodeHeight(E, P));
end

% 计算第1条通路的y坐标
H = caclNodeHeightOfEachPath(V, E, H, P{1});

% 首先计算第1条通路的分支的左下坐标
EdgeRectPos = initPathEdgeRectPos(EdgeRectPos, E, H, P{1});
E = removeFlow(E, P{1}, minWidth(1)); % 减去流量

% 扫描0流量分支
% 计算第1条通路的右上角坐标
zeroFlowEdges = findZeroFlowEdges(P{1}, E, theta);
EdgeRectPos = caclEdgesRectPos(EdgeRectPos, E, H, zeroFlowEdges, minWidth(1), 0);

% 绘制分支矩形块
if toDraw  % 是否需要绘制
    drawEdgeRect(zeroFlowEdges, EdgeRectPos, E);
    % 绘制第1条通路已绘制完成的分支文字标注
    drawEdgeAnnotation(zeroFlowEdges, EdgeRectPos, E);
end

for i=2:length(P)
    % 计算第i条通路的Y坐标
    H = caclNodeHeightOfEachPath(V, E, H, P{i});
    
    E2 = setdiff(P{i-1}, P{i});
    E3 = setdiff(P{i}, P{i-1});
    
    % 根据E3计算分支的左下角坐标
    count = i-1;
    EdgeRectPos = caclEdgesRectPos(EdgeRectPos, E, H, E3, caclEdgeFlowWidth(minWidth, count), 1);
    
    % 根据E2计算分支的右上角坐标
    EdgeRectPos = caclEdgesRectPos(EdgeRectPos, E, H, E2, caclEdgeFlowWidth(minWidth, count), 0);
    
    % 当前通路减去流量宽度
    E = removeFlow(E, P{i}, minWidth(i)); % 减去流量
    
    if toDraw  % 是否需要绘制
        % 绘制分支矩形块
        drawEdgeRect(E2, EdgeRectPos, E);
    end

    % 查找并标注被分割的分支(!!!!!important)
    spe = findSplitedEdges1(EdgeRectPos, E, E2, theta);
    splitedEdges = union(splitedEdges, spe); % 记录被分割的分支（将来可能用到!!!）
    
    if toDraw  % 是否需要绘制
        drawSplitedEdges(E, spe, EdgeRectPos); % 绘制分割分支颜色块
    end
    
    % 扫描通路，查找流量等于0的分支
    zeroFlowEdges = findZeroFlowEdges(P{i}, E, theta);
    EdgeRectPos = caclEdgesRectPos(EdgeRectPos, E, H, zeroFlowEdges, caclEdgeFlowWidth(minWidth, i), 0);
    
    if toDraw % 是否需要绘制
        % 绘制分支矩形块
        drawEdgeRect(zeroFlowEdges, EdgeRectPos, E);

        % 在0流量分支中查找已被标注为分割的分支(!!!!important)
        spe = findSplitedEdges2(splitedEdges, zeroFlowEdges);
        drawSplitedEdges(E, spe, EdgeRectPos);  % 绘制分割分支颜色块

        % 绘制分支文字标注
        % 与绘制分支矩形块分开绘制(drawEdgeRect)
        % 原因：可能将来需要调整文字的颜色，例如分割分支用红色，正常分支用其它颜色
        % 分开绘制，便于控制显示效果
        drawEdgeAnnotation(union(E2,zeroFlowEdges), EdgeRectPos, E);
    end
    
end

% 打印分支矩形块坐标
% celldisp(EdgeRectPos);

if toDraw
    % 绘制节点文字标注
    % drawVertexAnnotation(EdgeRectPos, V, VE, E);
end

% 将分割分支索引转换成实际的分支编号
b = E(1:end, 1);
splitedEdges = b(splitedEdges);
splitedEdges = splitedEdges'; % 列向量转化成行向量


function EdgeRectPos = initEdgeRectPos(E)
        % 初始化分支矩形块坐标
        % 格式：
        % EdgeRectPos =
        % {
        %   [10 20] [20 30];
        %   [20 40] [100 200];
        % };
        % 分支矩形块信息（左下角坐标和右上角坐标）
        EdgeRectPos={};
        for j = 1:size(E,1)
            EdgeRectPos{j,1} = []; % 左下角坐标
            EdgeRectPos{j,2} = []; % 右上角坐标
        end
    end

    function H = caclNodeHeight(V, E, from, P)
        % 计算节点压能
        H = zeros(1, length(V));        
        H(from) = 0; % 源点的y坐标为0
        
        for i = 1:length(P)
%             h = 0;
            for e = P{i} % 第i条独立通路
                u = E(e, 2);
                v = E(e, 3);                
                H(v) = H(u) + E(e, 5);
%                 h = h+E(e,5);
%                 disp(sprintf('分支e%d -- >H(%d)=%.4f \t H(%d)=%.4f\n', e, u, H(u), v, H(v)));
            end
%             disp(sprintf('第%d条独立通路的总高度%.3f\n', i, h));
        end
    end

    function maxH = caclMaxNodeHeight(E, P)
        % 计算节点压能
        H_all = [];
        for i = 1:length(P)
            H_all = [H_all sum(E(P{i}, 5))];
%             disp(sprintf('第%d条独立通路的总高度%.3f\n', i, H_all(i)));
        end

        % 2、求最大阻力
        maxH = max(H_all);
    end

    function outH = caclNodeHeightOfEachPath(V, E, H, path)
        % 计算每条独立通路的节点压能(也即纵坐标Y)
        for e = path
            u = E(e, 2);
            v = E(e, 3);
            H(v) = H(u) + E(e, 5);
        end
        outH = H;
    end

    function balanceVirtualEdgesH(V, E, P)
        % 平衡虚拟分支的阻力
        % 搜索出来的独立通路必然是不平衡的，构成了n-m+2个方程组，包含xxx个变量
        % 变量个数xxx == 增加的虚拟分支个数
        
        % 虚拟分支的个数
        Sn = length(find(V==-1)); % 虚拟源点出边分支
        Tn = length(find(E(1:end,1)==-1)); % 虚拟汇点入边分支
        
        disp(sprintf('虚拟分支个数 -- 出边：%d, 入边: %d\n', Sn, Tn));
        
        % 1、计算所有独立通路的阻力和                
        H_all = [];
         for i = 1:length(P)
             H_all = [H_all sum(E(P{i}, 5))];
            disp(sprintf('第%d条独立通路的总高度%.3f\n', i, H_all(i)));
         end
        
         % 2、求最大阻力
         maxH = max(H_all);
         disp(sprintf('最大阻力：%.3f\n', maxH));
         
         % 3、构建方程组矩阵AZ和b
         % 3.1需要映射变量的编号和矩阵列的位置
         colMap = [];
         for i = 1:size(E,1)
             if E(i,1) == -1
                 colMap = [colMap i];
             end
         end         
         disp(sprintf('列映射长度:%d\n', length(colMap)));
         
         % 3.2 构建方程组的系数矩阵AZ和
         AZ = zeros(size(E,1)-length(V)+2, length(colMap));
         b = [];
         for i = 1:length(P)
             path = P{i};
             
             % 查找通路的第一条分支(可能是虚分支)
             e = path(1);
             AZ(i, find(colMap==e)) = 1;
             
             % 查找通路的最后一条分支(可能是虚分支)             
             e = path(end);
             AZ(i, find(colMap==e)) = 1;
             
             b = [b maxH - H_all(i)];
         end
         
         BZ = [AZ';b]'; % 增广矩阵
                  
         % 4、求解方程组
         RA = rank(AZ);
         RB = rank(BZ);
         
         if RA ~= RB
             disp('方程组无解!');
             disp('列向量b:');
             disp(b);
             
             disp(AZ);
             disp(sprintf('矩阵RA的秩：%d\n', RA));
             disp(BZ)
             disp(sprintf('矩阵RB的秩：%d\n', RB));
         else
             % 方程组有解
             % 未实现！自行查找matlab求解非齐次方程组的资料
         end
    end

    function outE = removeFlow(E, path, q)
        % 通路path的分支减去部分流量
        for e = path
            E(e, 4) = E(e,4) - q;
        end
        outE = E;
    end

    function zeroFlowEdges = findZeroFlowEdges(path, E, theta)
        % 查找0流量的分支
        zeroFlowEdges = [];
        for e = path
            if abs(E(e, 4)) < theta % 使用微小变量作为判断浮点数相等的标准
                zeroFlowEdges = [zeroFlowEdges e];
            end
        end
    end

    function outEdgeRectPos = initPathEdgeRectPos(EdgeRectPos, E, H, path)
        % 初始化通路path中的分支的左下角坐标
        for e = path
            u = E(e, 2);
            EdgeRectPos{e, 1} = [0 H(u)];
        end
        outEdgeRectPos = EdgeRectPos;
    end

    function w = caclEdgeFlowWidth(minWidth, count)
        % 计算当前通路的累积流量宽度
        w = 0;
        for ii=1:count
            w = w + minWidth(ii); % 计算总的流量宽度
        end
    end

    function outEdgeRectPos = caclEdgesRectPos(EdgeRectPos, E, H, edges, w, isLeftCorner)   
        % 计算分支的坐标信息（通用函数）
        for e = edges
             u = E(e, 2);
             v = E(e, 3);
            if isLeftCorner % 计算左下角坐标
                EdgeRectPos{e, 1} = [w H(u)];
            else
                EdgeRectPos{e, 2} = [w H(v)];
            end
        end
        outEdgeRectPos = EdgeRectPos;
    end

    function drawVertexText(pt, v)
        % 绘制节点标注
        text(pt(1), pt(2), sprintf('v%d', v), 'HorizontalAlignment','center', 'BackgroundColor', [1 1 .6]);
    end

    function drawEdgeText(pt, e)
        % 绘制分支标注
        %text(pt(1), pt(2), sprintf('e%d', e), 'HorizontalAlignment','center', 'BackgroundColor', [.7 .9 .7]);
        text(pt(1), pt(2), sprintf('e%d', e), 'HorizontalAlignment','center');
    end

    function drawEdgeRect(edges, EdgeRectPos, E)
        % 绘制分支矩形块
        for e = edges
            if E(e,1) == -1 % 不绘制虚拟分支
                continue;
            end
            
            wh = EdgeRectPos{e,2} - EdgeRectPos{e,1}; % 计算矩形块的宽度和高度

             %绘制矩形，线宽LineWidth设置为1
            rectangle('Position', [EdgeRectPos{e,1} wh], 'LineWidth', 1);
            
            mid = (EdgeRectPos{e,2} + EdgeRectPos{e,1})/2; % 计算坐标的平均值
%             drawEdgeText(mid, E(e, 1));  % 绘制分支标注，例如e1、e2等
        end
    end

    function drawEdgeAnnotation(edges, EdgeRectPos, E)
        % 绘制分支文字标注
        for e = edges
            if E(e,1) == -1 % 不绘制虚拟分支
                continue;
            end
            
            mid = (EdgeRectPos{e,2} + EdgeRectPos{e,1})/2; % 计算坐标的平均值
            drawEdgeText(mid, E(e, 1));  % 绘制分支标注，例如e1、e2等
        end
    end

    function drawVertexAnnotation(EdgeRectPos, V, E)
        % 绘制节点文字标注
        function isFind = findElement(a, x)
            % 查找元素
            if isempty(find(a==x))
                isFind = false;
            else
                isFind = true;
            end
        end
        
        hasDrawedVertex=[]; % 已绘制的节点
        for e=1:size(E,1)
            if E(e,1) == -1
                continue; % 虚拟分支的节点不予绘制
            end
            
            u = E(e, 2); % 分支始节点
            v = VE(e, 3); % 分支末节点
            lbPt = EdgeRectPos{e,1};
            rtPt = EdgeRectPos{e,2};
            midWH = rtPt - lbPt;
            if ~findElement(hasDrawedVertex, u)
                drawVertexText(lbPt+[midWH(1)/2 0], V(u));
                hasDrawedVertex = [hasDrawedVertex u];
            end
            
            if ~findElement(hasDrawedVertex, v)
                drawVertexText(lbPt+[midWH(1)/2 midWH(2)], V(v));
                 hasDrawedVertex = [hasDrawedVertex v];
            end
        end
    end

    function setQHGraphDraw(totalFlow, totalPress)
        % 设置Q-H平衡图绘制参数
        % 需要考虑x、y轴的比例因子

        disp(sprintf('总流量：%.3f\t最大阻力:%.3f\n', totalFlow, totalPress));
        % 绘制Q-H平衡图之前的准备和设置工作
        xlabel('风量 Q','FontSize',12);
        ylabel('风压 H','FontSize',12);
        %title('xxx矿Q-H平衡图','FontSize',16);
        
        % 设置x-y轴的区间范围，这里只是简单的设置一个数值
        % 正常情况下，应该是根据实际的总流量和总阻力进行计算的    
        % 设置Q-H坐标轴的范围       
        x0 = -1;
        y0 = -totalPress/totalFlow;
        xmax = totalFlow+20;
        ymax = totalPress+20;
                
        daspect([1 totalPress/totalFlow 1]); % 设置比例
        axis([x0, xmax, y0, ymax]);
    end

    function splitedEdges = findSplitedEdges1(EdgeRectPos, E, edges, theta)
        % 查找被分割的分支()
        % 查找方法：流量还有剩余，但已经绘制完成
        splitedEdges = [];
        for e = edges
            if isempty(EdgeRectPos{e,2}) | E(e,4)<theta  % 只选择已确定坐标，且剩余流量大于0的分支
                continue;
            end

            b = (EdgeRectPos{e,2}-EdgeRectPos{e,1})>0;  % 判断右上角坐标是否大于左下角坐标
            if sum(b)==2
                splitedEdges = [splitedEdges, e];
            end
        end
    end

    function splitedEdges = findSplitedEdges2(inSplitedEdges, edges)
        % 查找被分割的分支（在0流量的分支中查找)
        % 查找方法：分支已标记为被分割        
        splitedEdges = intersect(edges, inSplitedEdges);        
    end

    function drawColorBlock(lbPt, rtPt)
        % 绘制颜色块（这里采用cyan）
        wh = rtPt -lbPt;
        if wh(1)<0 | wh(2)<0
            return;
        end
        rectangle('Position', [lbPt wh], 'FaceColor', 'c');
    end

    function drawSplitedEdges(E, splitedEdges, EdgeRectPos)
        % 采用红色标注被分割的分支
        for e = splitedEdges
            if E(e, 1) == -1
                continue;
            end
            drawColorBlock(EdgeRectPos{e,1}, EdgeRectPos{e,2}); % 绘制颜色块
        end
    end
end