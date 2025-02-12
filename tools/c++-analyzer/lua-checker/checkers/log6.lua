local utils = require'utils'

local Log6, LogIds, NewLinePos

do
    local peg = utils.peg
    local C = peg.C
    local P = peg.P
    local S = peg.S
    local V = peg.V
    local Ct = peg.Ct
    local Cp = peg.Cp()
    local After = peg.After
    local Until = peg.Until
    local ws0 = peg.ws0

    local logid = P'LogId::'
    local idname = C(peg.word)
    local kvlog = P'KVLog'
    local WithinBalanced = ((1 - S'()') + V'Balanced')^0 * ')'
    local Balanced = '(' * WithinBalanced
    local LogId = logid * idname
    local KVArgs = kvlog * '("' * C((1-P'"')^1) * WithinBalanced

    Log6 = P{
        Ct((
            Until(P'log6(' + P'executable_log6_if(' * (P'EXECUTABLE_LOG6_ID_AND_NAME(' + '"'))
          * (
            -- log6(LogId::{ID}, KVLogList{KVLog({key}, value), ...})
            -- log6(cond ? LogId::{ID} : LogId::{ID}, KVLogList{KVLog({key}, value), ...})
              'log6('
              * Ct( Cp
                  * Ct((Until(logid+kvlog+';') * LogId)^1)
                  * Ct((Until(kvlog+';') * KVArgs)^0))
            -- executable_log6_if(EXECUTABLE_LOG6_ID_AND_NAME({ID}), {key}, ...)
            + 'executable_log6_if(EXECUTABLE_LOG6_ID_AND_NAME'
              * Ct( Cp
                  * '(' * Ct(idname) * ')'
                  * Ct((',' * ws0 * '"' * idname * '"_av')^0))
            -- executable_log6_if("{ID}"_ascii_upper, LogId::{key}, ...)
            + 'executable_log6_if("'
              * Ct( Cp
                  * After('"')
                  * '_ascii_upper, LogId::' * Ct(idname)
                  * Ct((',' * ws0 * '"' * idname * '"_av')^0))
            -- no match
            + 1
          )
        )^0),
        Balanced = Balanced,
    }

    NewLinePos = Ct(After('\n' * Cp)^0 * Cp)

    LogIds = Ct((After(P'  f(') * idname)^0)
end


local lower_bound = function(t, value, ibegin, iend)
    local count, i = iend-ibegin
    while count > 0 do
        step = math.floor(count / 2)
        i = ibegin + step
        if t[i] < value then
            i = i + 1
            ibegin = i
            count = count - step + 1
        else
            count = step
        end
    end
    return ibegin
end


local ids = {}
local verbose = false

function init(args)
    if args == 'verbose' then
        verbose = true
    end

    local i = 0
    for _,v in ipairs(LogIds:match(utils.readall('src/core/log_id.hpp'))) do
        ids[v] = 0
        i = i + 1
    end
    assert(i > 10) -- random magic number
end

local logs = {}
function file(content, filename)
    local r = Log6:match(content)
    if r then
        poslines = NewLinePos:match(content)

        local iline = 1
        for _,t in pairs(r) do
            iline = lower_bound(poslines, t[1], iline, #poslines+1)
            strkvlog = table.concat(t[3], ', ')
            for _,id in pairs(t[2]) do
                logs[#logs+1] = {filename, iline, id, strkvlog}
            end
        end
    end
end

function string.starts_with(self, subs)
    if #self == #subs then return self == subs end
    if #self < #subs then return false end
    return self:sub(0, #subs) == subs
end

function check_prefix_parameters(kvs_list, kvs_log)
    for _, s in ipairs(kvs_list) do
        if not s:starts_with(kvs_log) and not kvs_log:starts_with(s) then
            return false
        end
    end
    return true
end

function terminate()
    if #logs == 0 then
        utils.print_error('log6(...) not found\n')
        return 1
    end

    table.sort(logs, function(a, b) return a[3] < b[3] end)

    function printlog(info)
        utils.print_error(string.format('%s:%d:\n  %s  %s\n', info[1], info[2], info[3], info[4]))
    end

    if verbose then
        for _,v in ipairs(logs) do
            printlog(v)
        end
    end

    local previouslog = logs[1]
    local errcount = 0
    local kvs_list = {}
    ids[previouslog[3]] = 1
    for i=2,#logs do
        log = logs[i]
        ids[log[3]] = 1
        -- when id differ
        if previouslog[3] ~= log[3] then
            kvs_list = {}
        -- when kv log differ from prefix
        elseif not check_prefix_parameters(kvs_list, log[4]) then
            utils.print_error('--- log6 differ:\n')
            printlog(previouslog)
            printlog(logs[i])
            errcount = errcount + 1
        end
        kvs_list[#kvs_list+1] = log[4]
        previouslog = log
    end

    -- add not extracted id
    -- NOTE synchronize with tools/log_siem_extractor.py
    for _,id in ipairs({
        'PROBE_STATUS',
        -- special case in tools/log_siem_extractor.py (server_cert_regex)
        -- @{
        'SERVER_CERTIFICATE_ERROR',
        'SERVER_CERTIFICATE_MATCH_SUCCESS',
        'CERTIFICATE_CHECK_SUCCESS',
        'SERVER_CERTIFICATE_NEW',
        'SERVER_CERTIFICATE_MATCH_FAILURE',
        -- @}
        -- obsolete, not used
        -- @{
        'DRIVE_REDIRECTION_READ_EX',
        'CB_COPYING_PASTING_DATA_TO_REMOTE_SESSION_EX',
        'OUTBOUND_CONNECTION_DETECTED_2',
        'OUTBOUND_CONNECTION_BLOCKED_2',
        'EDIT_CHANGED_2',
        'STARTUP_APPLICATION_FAIL_TO_RUN_2',
        'DRIVE_REDIRECTION_WRITE_EX',
        'CB_COPYING_PASTING_DATA_FROM_REMOTE_SESSION_EX',
        -- @}
    }) do
        if ids[id] ~= 0 then
            utils.print_error(id .. ' is already used, please update script\n')
            errcount = errcount + 1
        else
            ids[id] = 1
        end
    end

    for k,v in pairs(ids) do
        if v ~= 1 then
            utils.print_error('LogId::' .. k .. ' not used\n')
            errcount = errcount + 1
        end
    end

    return errcount
end


return {init=init, file=file, terminate=terminate}
