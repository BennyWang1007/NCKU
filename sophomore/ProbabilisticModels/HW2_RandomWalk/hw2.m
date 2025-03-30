function main()

    TO_SAVE = 1;
    
    % Parameters
    % dimensions = [1, 2, 3, 4];
    dimensions = [1];
    steps = [100, 1000, 10000, 100000, 1000000];
    walk_count = 1000;
    
    % Create results directory if it doesn't exist
    if ~exist('results', 'dir')
        mkdir('results');
    end
    
    for d = 1:length(dimensions)
        dim = dimensions(d);
        for s = 1:length(steps)
            % Record execution time
            tic;

            step = steps(s);
            
            fprintf('Dimension: %d, Steps: %d\n', dim, step);
            
            randomWalk = RandomWalk(walk_count, dim, step);
            randomWalk = randomWalk.walk();
            
            % Get results
            positions = randomWalk.getPosition();
            sections_count = randomWalk.getSectionsCount();
            origin_steps = randomWalk.getOriginSteps();
            origin_step_count = randomWalk.getOriginStepCount();

            if TO_SAVE == 1
                % Save positions to file
                filename = sprintf('results/positions_dim%d_step%d.csv', dim, step);
                dlmwrite(filename, positions, 'delimiter', ',', 'precision', 16);
                
                % Save sections count to file
                filename = sprintf('results/sections_dim%d_step%d.csv', dim, step);
                section_data = zeros(length(sections_count), 2);
                for i = 1:length(sections_count)
                    section_data(i, 1) = i-1;  % 0-based index as in C++
                    section_data(i, 2) = sections_count(i);
                end
                dlmwrite(filename, section_data, 'delimiter', ',', 'precision', 16);
            end
            
            % Process and save origin steps
            if origin_step_count > 0
                % Sort origin steps
                origin_steps = sort(origin_steps);
                
                % Calculate average
                avg_origin_step = mean(origin_steps);
                fprintf('Average: %f\n', avg_origin_step);
                
                if TO_SAVE == 1
                    % Save origin steps to file
                    % filename = sprintf('results/origin_steps_dim%d_step%d.csv', dim, step);
                    % dlmwrite(filename, origin_steps', 'delimiter', ',', 'precision', 16);
                    % save as a dictionary: key = step, value = count
                    filename = sprintf('results/origin_steps_dim%d_step%d.csv', dim, step);
                    origin_step_data = zeros(length(unique(origin_steps)), 2);
                    % print origin steps and their counts
                    
                    [unique_steps, ~, ic] = unique(origin_steps);
                    counts = accumarray(ic, 1);
                    for i = 1:length(unique_steps)
                        origin_step_data(i, 1) = unique_steps(i);
                        origin_step_data(i, 2) = counts(i);
                    end
                    % origin_steps
                    % unique_steps
                    % origin_step_data
                    % assert(0);
                    dlmwrite(filename, origin_step_data, 'delimiter', ',', 'precision', 16);
                
                    % Plot histogram of origin steps
                    figure;
                    histogram(origin_steps, min(100, length(unique(origin_steps))));
                    % histogram(origin_steps, length(unique(origin_steps)));
                    title(sprintf('Histogram of Origin Steps (Dim=%d, Steps=%d)', dim, step));
                    xlabel('Steps between Origin Visits');
                    ylabel('Frequency');
                    
                    % Save the histogram
                    filename = sprintf('results/histogram_origin_steps_dim%d_step%d.png', dim, step);
                    saveas(gcf, filename);
                    close(gcf);
                end
            else
                fprintf('No origin steps\n');
            end
            
            % m/n ratio for dimension 1
            if dim == 1
                n_plus_count = randomWalk.getNPlusCount();
                n_minus_count = randomWalk.getNMinusCount();
                n_zero_count = randomWalk.getNZeroCount();
                
                m_divide_n = (n_zero_count/2.0 + max(n_plus_count, n_minus_count)) ./ step;
                
                if TO_SAVE == 1

                    % Save m/n ratio to file
                    filename = sprintf('results/m_divide_n_dim%d_step%d.csv', dim, step);
                    dlmwrite(filename, m_divide_n', 'delimiter', ',', 'precision', 16);

                    % save n+, n-, n0 to file
                    filename = sprintf('results/m_divide_n_counts_dim%d_step%d.csv', dim, step);
                    m_divide_n_counts = zeros(length(m_divide_n), 3);
                    m_divide_n_counts(:, 1) = n_plus_count;
                    m_divide_n_counts(:, 2) = n_minus_count;
                    m_divide_n_counts(:, 3) = n_zero_count;
                    dlmwrite(filename, m_divide_n_counts, 'delimiter', ',', 'precision', 16);
                    
                    % Plot histogram of m/n ratio
                    figure;
                    histogram(m_divide_n, min(50, length(unique(m_divide_n))));
                    title(sprintf('Histogram of m/n Ratio (Dim=%d, Steps=%d)', dim, step));
                    xlabel('m/n Ratio');
                    ylabel('Frequency');
                    
                    % Save the histogram
                    filename = sprintf('results/histogram_m_divide_n_dim%d_step%d.png', dim, step);
                    saveas(gcf, filename);
                    close(gcf);

                    % Plot histogram of n+
                    figure;
                    histogram(n_plus_count, min(50, length(unique(n_plus_count))));
                    title(sprintf('Histogram of n+ (Dim=%d, Steps=%d)', dim, step));
                    xlabel('n+');
                    ylabel('Frequency');

                    % Save the histogram
                    filename = sprintf('results/histogram_n_plus_dim%d_step%d.png', dim, step);
                    saveas(gcf, filename);
                    close(gcf);

                    % Plot histogram of n-
                    figure;
                    histogram(n_minus_count, min(50, length(unique(n_minus_count))));
                    title(sprintf('Histogram of n- (Dim=%d, Steps=%d)', dim, step));
                    xlabel('n-');
                    ylabel('Frequency');

                    % Save the histogram
                    filename = sprintf('results/histogram_n_minus_dim%d_step%d.png', dim, step);
                    saveas(gcf, filename);
                    close(gcf);

                    % Plot histogram of n0
                    figure;
                    histogram(n_zero_count, min(50, length(unique(n_zero_count))));
                    title(sprintf('Histogram of n0 (Dim=%d, Steps=%d)', dim, step));
                    xlabel('n0');
                    ylabel('Frequency');

                    % Save the histogram
                    filename = sprintf('results/histogram_n_zero_dim%d_step%d.png', dim, step);
                    saveas(gcf, filename);
                    close(gcf);

                    % plot histogram of max(n+, n-) / n
                    figure;
                    histogram((n_zero_count/2.0 + max(n_plus_count, n_minus_count)) ./ step, min(50, length(unique(m_divide_n))));
                    title(sprintf('Histogram of max(n+, n-) / n (Dim=%d, Steps=%d)', dim, step));
                    xlabel('max(n+, n-) / n');
                    ylabel('Frequency');

                    % Save the histogram
                    filename = sprintf('results/histogram_max_n_plus_minus_divide_n_dim%d_step%d.png', dim, step);
                    saveas(gcf, filename);
                    close(gcf);

                    % save a plot of right side of n+ (n+ > n / 2)
                    figure;
                    histogram(n_plus_count(n_plus_count > step / 2), min(50, length(unique(n_plus_count(n_plus_count > step / 2)))));
                    title(sprintf('Histogram of n+ > n/2 (Dim=%d, Steps=%d)', dim, step));
                    xlabel('n+');
                    ylabel('Frequency');

                    % Save the histogram
                    filename = sprintf('results/histogram_n_plus_right_side_dim%d_step%d.png', dim, step);
                    saveas(gcf, filename);
                    close(gcf);

                end
            end

            % Report execution time
            elapsed_time = toc;
            fprintf('Dimension: %d, Steps: %d, Elapsed time: %f seconds\n\n', dim, step, elapsed_time);
        end
    end
    
    % Report execution time
    elapsed_time = toc;
    fprintf('Elapsed time: %f seconds\n', elapsed_time);
end

% Run the main function
main();