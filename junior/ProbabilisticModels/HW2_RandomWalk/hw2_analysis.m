
function partitions = generate_even_partitions(n)
    % Generates all ways to partition n into even numbers greater than 0.
    partitions = cell(1, 2^(n/2 - 1)); % reserve 2 ^ (n/2 - 1) space for partitions
    [partitions, ~] = generate([], n, partitions, 1);
end


function [partitions, current_idx] = generate(current, remaining, partitions, current_idx)
    if remaining == 0
        % partitions{end + 1} = current;
        partitions{current_idx} = current;
        current_idx = current_idx + 1;
        return;
    end
    for i = 2:2:remaining
        [partitions, current_idx] = generate([current, i], remaining - i, partitions, current_idx);
    end
end


function [ret, cache] = step_back_to_origin_analysis_formula_1d(diff_sep, cache)
    if diff_sep == 0
        ret = 1;
        return;
    end
    if cache(diff_sep) ~= 0
        ret = cache(diff_sep);
        return;
    end

    p = nchoosek(diff_sep, diff_sep/2) / 2^diff_sep;
    partitions = generate_even_partitions(diff_sep);
    for i = 1:length(partitions)
        partition = partitions{i};
        if length(partition) == 1
            continue;
        end
        product = 1;
        for j = 1:length(partition)
            [pdiff, cache] = step_back_to_origin_analysis_formula_1d(partition(j), cache);
            product = product * pdiff;
        end
        p = p - product;
    end
    cache(diff_sep) = p;
    ret = p;
end


function [ret, cache] = step_back_to_origin_analysis_formula(dim, diff_sep, cache, partitions_cell)
    if diff_sep == 0
        ret = 1;
        return;
    end
    if cache(diff_sep) ~= 0
        ret = cache(diff_sep);
        return;
    end

    p = (nchoosek(diff_sep, diff_sep/2) / 2^diff_sep) ^ dim;
    % partitions = generate_even_partitions(diff_sep);
    partitions = partitions_cell{diff_sep + 1};
    for i = 1:length(partitions)
        partition = partitions{i};
        if length(partition) == 1
            continue;
        end
        product = 1;
        for j = 1:length(partition)
            [pdiff, cache] = step_back_to_origin_analysis_formula(dim, partition(j), cache, partitions_cell);
            product = product * pdiff;
        end
        p = p - product;
    end
    cache(diff_sep) = p;
    ret = p;
end


function step_back_to_origin_analysis(dim, steps, partitions)
    % formula = zeros(1, 50); % Used as a cache for the formula
    for s = 1:length(steps)
        step = steps(s);
        filename = sprintf('results/origin_steps_dim%d_step%d.csv', dim, step);
        origin_steps = readmatrix(filename); % format [step, count]
        origin_steps = repelem(origin_steps(:, 1), origin_steps(:, 2));

        % [~, formula] = step_back_to_origin_analysis_formula(dim, 50, formula, partitions);
        % condensed_formula = formula(2:2:end);
        % disp(condensed_formula);
        % fprintf('Time: %f\n', end_t);

        condensed_formulas = cell(1, 4);
        condensed_formulas{1} = [0.5000, 0.1250, 0.0625, 0.0391, 0.0273, 0.0205, 0.0161, 0.0131, 0.0109, ...
            0.0093, 0.0080, 0.0070, 0.0062, 0.0055, 0.0050, 0.0045, 0.0041, 0.0038, ...
            0.0035, 0.0032, 0.0030, 0.0028, 0.0026, 0.0024, 0.0023];
        condensed_formulas{2} = [0.2500, 0.0781, 0.0430, 0.0286, 0.0210, 0.0164, 0.0134, 0.0112, 0.0096, ...
            0.0083, 0.0073, 0.0066, 0.0059, 0.0054, 0.0049, 0.0045, 0.0042, 0.0039, ...
            0.0036, 0.0034, 0.0032, 0.0030, 0.0028, 0.0027, 0.0026];
        condensed_formulas{3} = [0.1250, 0.0371, 0.0193, 0.0123, 0.0087, 0.0065, 0.0052, 0.0042, 0.0035, ...
            0.0030, 0.0026, 0.0023, 0.0020, 0.0018, 0.0016, 0.0015, 0.0013, 0.0012, ...
            0.0011, 0.0010, 0.0010, 0.0009, 0.0008, 0.0008, 0.0007];
        condensed_formulas{4} = [0.0625, 0.0159, 0.0073, 0.0042, 0.0028, 0.0019, 0.0014, 0.0011, 0.0009, ...
            0.0007, 0.0006, 0.0005, 0.0004, 0.0004, 0.0003, 0.0003, 0.0003, 0.0002, ...
            0.0002, 0.0002, 0.0002, 0.0002, 0.0001, 0.0001, 0.0001];

        condensed_formula = condensed_formulas{dim};


        sum_of_condensed_formula = sum(condensed_formula);
        % disp(sum_of_condensed_formula);

        % plot histogram with x = 2, 4, 6, ..., 50, condiser all steps for probability but don't plot steps > 50
        probs_for_steps = zeros(1, 25);
        for i = 2:2:50
            probs_for_steps(i/2) = sum(origin_steps == i) / length(origin_steps);
        end
        if dim > 1
            probs_for_steps = probs_for_steps * sum_of_condensed_formula;
        end

        % plot the histogram
        figure;
        bar(2:2:50, probs_for_steps, 'BarWidth', 1);
        title(sprintf('Dimension: %d, Steps: %d', dim, step));
        xlabel('Steps to return to origin');
        ylabel('Probability');
        
        % plot the formula
        hold on;
        plot(2:2:50, condensed_formula, 'r');
        hold off;

        % wait for user input
        input('Press Enter to continue...');

        % save the plot as a png file and fig file at processed_plot/histogram_origin_steps_dim{dim}_step{step}_discard50.png/fig
        filename = sprintf('processed_plot/histogram_origin_steps_dim%d_step%d_discard50', dim, step);
        saveas(gcf, filename, 'png');
        saveas(gcf, filename, 'fig');
        close all;
    end
end


function plot_step_back_to_origin_analysis_formula(test_max_idx, partitions)

    dims = [1, 2, 3, 4];
    caches = cell(1, length(dims));
    for i = 1:length(dims)
        cache = zeros(1, test_max_idx);
        [~, cache] = step_back_to_origin_analysis_formula(dims(i), test_max_idx, cache, partitions);
        caches{i} = cache(2:2:end);
    end

    % plot caches
    figure;
    for i = 1:length(dims)
        plot(2:2:test_max_idx, caches{i});
        hold on;
    end
    hold off;
    legend('1D', '2D', '3D', '4D');
    title('Probability of Returning to Origin');
    xlabel('Steps');
    ylabel('Probability');
end


function plot_experiment_of_m_divide_n(dim, steps)
    for s = 1:length(steps)
        step = steps(s);
        filename = sprintf('results/m_divide_n_dim%d_step%d.csv', dim, step);
        m_divide_n = readmatrix(filename);

        % plot histogram with m/n ratio
        figure;
        histogram(m_divide_n, 'Normalization', 'probability', 'BinWidth', 0.01);
        title(sprintf('Dimension: %d, Steps: %d', dim, step));
        xlabel('m/n Ratio');
        ylabel('Probability');

        % wait for user input
        input('Press Enter to continue...');

        % save the plot as a png file at processed_plot/histogram_m_divide_n_dim{dim}_step{step}.png
        filename = sprintf('processed_plot/histogram_m_divide_n_dim%d_step%d', dim, step);
        saveas(gcf, filename, 'png');
        saveas(gcf, filename, 'fig');

        close all;
    end
end


% ----------------- Main -----------------


% ----------------- Initialize -----------------
test_max_idx = 50;

% Only need to run once
partitions = cell(1, test_max_idx + 1);  % Cache for partitions of each number
partitions{1} = {[]};  % Base case: partition of 0 is an empty set
for i=2:2:test_max_idx
    partitions{i + 1} = generate_even_partitions(i);
end


% ----------------- Origin's Test -----------------

dim = [1, 2, 3, 4];
steps = [100, 1000, 10000, 100000, 1000000];

for i = 1:length(dim)
    step_back_to_origin_analysis(dim(i), steps, partitions);
end

plot_step_back_to_origin_analysis_formula(test_max_idx, partitions);


%  ----------------- Section's Test -----------------

function draw_sections(dim, step)
    filename = sprintf('results/sections_dim%d_step%d.csv', dim, step);
    sections_data = readmatrix(filename);
    sections = sections_data(:, 1);
    sections_count = sections_data(:, 2);

    figure;
    % histogram(sections, 'Normalization', 'probability', 'BinWidth', 1);
    bar(sections, sections_count / sum(sections_count));
    title(sprintf('Dimension: %d, Steps: %d', dim, step));
    xlabel('Sections');
    ylabel('Probability');

    % wait for user input
    input('Press Enter to continue...');

    % save the plot as a png file at processed_plot/histogram_sections_dim{dim}_step{step}.png
    filename = sprintf('processed_plot/histogram_sections_dim%d_step%d', dim, step);
    saveas(gcf, filename, 'png');

    close all;
end

dims = [1, 2, 3, 4];
steps = [100, 1000, 10000, 100000, 1000000];
for i = 1:length(dims)
    for j = 1:length(steps)
        draw_sections(dims(i), steps(j));
    end
end


% ----------------- L1 and L2 norm's Test -----------------
function draw_l1_norm(dim, step)
    filename = sprintf('results/positions_dim%d_step%d.csv', dim, step);
    positions = readmatrix(filename);
    L1_norm = sum(abs(positions), 2);

    figure;
    max_L1 = max(L1_norm);
    max_bins_count = 100;
    bins_width = max_L1 / max_bins_count;
    histogram(L1_norm, 'Normalization', 'probability','BinLimits', [0, max_L1], 'BinWidth', bins_width);
    title(sprintf('Dimension: %d, Steps: %d', dim, step));
    xlabel('L1 Norm');
    ylabel('Probability');

    % wait for user input
    input('Press Enter to continue...');

    % save the plot as a png file at processed_plot/scatter_L1_norm_dim{dim}_step{step}.png
    filename = sprintf('processed_plot/histogram_L1_norm_dim%d_step%d', dim, step);
    saveas(gcf, filename, 'png');

    close all;
end

function draw_l2_norm(dim, step)
    filename = sprintf('results/positions_dim%d_step%d.csv', dim, step);
    positions = readmatrix(filename);
    L2_norm = sqrt(sum(positions .^ 2, 2));

    figure;
    max_L2 = max(L2_norm);
    max_bins_count = 100;
    bins_width = max_L2 / max_bins_count;
    histogram(L2_norm, 'Normalization', 'probability','BinLimits', [0, max_L2], 'BinWidth', bins_width);
    title(sprintf('Dimension: %d, Steps: %d', dim, step));
    xlabel('L2 Norm');
    ylabel('Probability');

    % % draw a chi-square distribution with degree of freedom = dim
    % hold on;
    % mul = sqrt(step)/4;
    % x = 0:0.01:max_L2/mul;
    % y = chi2pdf(x, 2*dim);
    % x = x * mul;
    % plot(x, y * sqrt(step), 'r');
    % hold off;

    % wait for user input
    input('Press Enter to continue...');

    % save the plot as a png file at processed_plot/scatter_L2_norm_dim{dim}_step{step}.png
    filename = sprintf('processed_plot/histogram_L2_norm_dim%d_step%d', dim, step);
    saveas(gcf, filename, 'png');

    close all;
end

dims = [1, 2, 3, 4];
steps = [100, 1000, 10000, 100000, 1000000];
for i = 1:length(dims)
    for j = 1:length(steps)
        draw_l1_norm(dims(i), steps(j));
        draw_l2_norm(dims(i), steps(j));
    end
end

%  ----------------- m/n Ratio's Test -----------------
