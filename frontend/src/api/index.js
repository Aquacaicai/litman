import * as articleApi from './articleApi';
import * as authorApi from './authorApi';
import * as searchApi from './searchApi';
import * as statsApi from './statsApi';
import * as importApi from './importApi';
import { setApiRoot } from './config';
import config from './config';

export default {
    article: articleApi,
    author: authorApi,
    search: searchApi,
    stats: statsApi,
    import: importApi,

    config: {
        get apiRoot() {
            return config.apiRoot;
        },
        setApiRoot
    }
};
