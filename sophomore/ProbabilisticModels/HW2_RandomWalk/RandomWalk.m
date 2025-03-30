classdef RandomWalk
    properties (Access = private)
        test_count
        dimension
        step_count
        
        position        % position of each test
        sections_count  % count of each sections
        % prev_orgin_step % previous origin step
        origin_steps    % steps between two origin steps
        origin_step_count % number of origin steps
        n_plus_count    % count of n+
        n_minus_count   % count of n-
        n_zero_count    % count of n0
        
        gen             % random number generator
    end
    
    methods
        function obj = RandomWalk(test_count, dimension, step_count)

            time_start = tic;

            obj.test_count = test_count;
            obj.dimension = dimension;
            obj.step_count = step_count;
            
            obj.position = zeros(test_count, dimension);
            obj.sections_count = zeros(1, 2^dimension);
            % obj.prev_orgin_step = 0;
            obj.origin_steps = zeros(1, test_count * step_count);
            % obj.origin_steps = [];
            obj.origin_step_count = 0;
            
            if dimension == 1
                obj.n_plus_count = zeros(1, test_count);
                obj.n_minus_count = zeros(1, test_count);
                obj.n_zero_count = zeros(1, test_count);
            end
            
            % Set fixed seed for reproducibility
            rng(42);
            obj.gen = RandStream.getGlobalStream;

            time_end = toc(time_start);
            % fprintf('RandomWalk object created in %f seconds\n', time_end);
        end
        
        function obj = walk(obj)
            for test_idx = 1:obj.test_count
                random_nums = randi(obj.gen, [0, 1], obj.step_count, obj.dimension);
                random_nums(random_nums == 0) = -1;
                prev_orgin_step = 0;
                current_pos = zeros(1, obj.dimension);
                % pos_history = zeros(obj.step_count, obj.dimension);
                
                for j = 1:obj.step_count
                    current_pos = current_pos + random_nums(j, :);

                    % collect data
                    
                    % check if has zero
                    if any(current_pos == 0)
                        at_origin = all(current_pos == 0);
                    else
                        at_origin = false;
                        % record the section of the position
                        section_index = 0;
                        for k = 1:obj.dimension
                            if current_pos(k) > 0
                                section_index = section_index + 2^(k - 1);
                            end
                        end
                        obj.sections_count(section_index + 1) = obj.sections_count(section_index + 1) + 1;
                    end

                    % record the steps between two origin steps
                    if at_origin
                        obj.origin_step_count = obj.origin_step_count + 1;
                        obj.origin_steps(obj.origin_step_count) = j - prev_orgin_step;
                        if j == prev_orgin_step
                            assert(false);
                        end
                        % obj.origin_steps = [origin_steps, j - prev_orgin_step];
                        prev_orgin_step = j;
                    end

                    % record n+, n-, n0
                    if obj.dimension == 1
                        if current_pos(1) > 0
                            obj.n_plus_count(test_idx) = obj.n_plus_count(test_idx) + 1;
                        elseif current_pos(1) < 0
                            obj.n_minus_count(test_idx) = obj.n_minus_count(test_idx) + 1;
                        else
                            obj.n_zero_count(test_idx) = obj.n_zero_count(test_idx) + 1;
                        end
                    end
                    
                end
                obj.position(test_idx, :) = current_pos;
            end
            if obj.origin_step_count > 0
                obj.origin_steps = obj.origin_steps(1:obj.origin_step_count);
            else
                obj.origin_steps = [];
            end
        end
        
        function obj = collect_data(obj, pos, step, test_num)
            has_zero = any(pos == 0);
            
            if has_zero
                at_origin = all(pos == 0);
            else
                at_origin = false;
                % record the section of the position
                section_index = 0;
                for i = 1:obj.dimension
                    if pos(i) > 0
                        section_index = section_index + 2^(i - 1);
                    end
                end
                obj.sections_count(section_index + 1) = obj.sections_count(section_index + 1) + 1;
            end
            
            % record the steps between two origin steps
            if at_origin
                obj.origin_step_count = obj.origin_step_count + 1;
                obj.origin_steps = [obj.origin_steps, step - obj.prev_orgin_step];
                % obj.origin_steps(obj.origin_step_count) = step - obj.prev_orgin_step;
                obj.prev_orgin_step = step;
            end
            
            % record n+, n-, n0
            if obj.dimension == 1
                if pos(1) > 0
                    obj.n_plus_count(test_num) = obj.n_plus_count(test_num) + 1;
                elseif pos(1) < 0
                    obj.n_minus_count(test_num) = obj.n_minus_count(test_num) + 1;
                else
                    obj.n_zero_count(test_num) = obj.n_zero_count(test_num) + 1;
                end
            end
        end
        
        function positions = getPosition(obj)
            positions = obj.position;
        end
        
        function counts = getSectionsCount(obj)
            counts = obj.sections_count;
        end
        
        function steps = getOriginSteps(obj)
            steps = obj.origin_steps;
        end
        
        function count = getOriginStepCount(obj)
            count = obj.origin_step_count;
        end
        
        function counts = getNPlusCount(obj)
            counts = obj.n_plus_count;
        end
        
        function counts = getNMinusCount(obj)
            counts = obj.n_minus_count;
        end
        
        function counts = getNZeroCount(obj)
            counts = obj.n_zero_count;
        end
    end
end
